#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       httpd.py
Create Time:    2015-07-01 23:37
Description:
    python version tiny httpd for study purpose
ToDo:
    keep-alive issue
    python -m SimpleHTTPServer 8000, see the difference and modify it.
Memo:
    pep8
    autopep8 check.
    yapf check.
"""

__version__ = "0.1"

import argparse
import os
import posixpath
import stat
import socket
import fcntl
import re
import struct
import time
import select
import Queue
import threading
import sys
import time
import logging
import mimetypes
import errno
import StringIO
import gzip
import signal

import reactor

#FILEPATH = '/home/charlie/tinyhttpd/'
FILEPATH = "/home/charlie/swift/doc/build/html"
GET = 'GET'
POST = 'POST'
HEAD = 'HEAD'


def compact_tracebak():
    """format exception output"""
    t, v, tb = sys.exc_info()
    tbinfo = []
    if not tb:
        raise AssertionError("traceback does not exist")
    while tb:
        tbinfo.append((
            tb.tb_frame.f_code.co_filename,
            tb.tb_frame.f_code.co_name,
            str(tb.tf_lineno),
        ))
        tb = tb.tb_next
    del tb
    file, funcion, line = tbinfo[-1]
    info = ' '.join('[%s|%s|%s]' % x for x in tbinfo)
    return (file, funcion, line), t, v, info

# try:
#    0=1
# except:
#    print compact_tracebak()


def config_log():
    logger = logging.getLogger('out')
    logger.setLevel(logging.DEBUG)

    fh = logging.FileHandler("./httpd.log")
    fh.setLevel(logging.INFO)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)
    #ch.setLevel(logging.ERROR)

    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    formatter_full = logging.Formatter(
        '%(asctime)s - %(process)d - %(levelname)s - %(filename)s:%(lineno)d - %(message)s')
    ch.setFormatter(formatter_full)
    fh.setFormatter(formatter)

    logger.addHandler(ch)
    # logger.addHandler(fh)

    return logger


def parse_argument():
    parser = argparse.ArgumentParser(
        description='tiny httpd server, written in python',
        epilog='example: %(prog)s -i 127.0.0.1 -p 8080 ')

    parser.add_argument('-i', '--ip',
                        metavar='ip',
                        # nargs=1,
                        type=str, default='127.0.0.1',
                        help='ip address')

    parser.add_argument('-p', '--port',
                        metavar='port',
                        # nargs=1,
                        type=int, default=8080,
                        help='port')

    return parser.parse_args()


def singleton(cls, *args, **kwargs):
    """singleton mode, used for decorator"""
    instances = {}

    def _singleton(*args, **kwargs):
        if cls not in instances:
            instances[cls] = cls(*args, **kwargs)
        return instances[cls]
    return _singleton


def daemonize(stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
    """daemonize the process, no need to start the process in nohup way"""
    
    def handle_exit(signum, _):
        if signum == signal.SIGTERM:
            sys.exit(0)

    signal.signal(signal.SIGINT, handle_exit)
    signal.signal(signal.SIGTERM, handle_exit)

    try:
        pid = os.fork()
        assert pid != -1 
        if pid > 0:
            time.sleep(5)
            sys.exit(0)
    except OSError, e:
        logger.error("fork #1 fail, errno:{0} {1}".format(e.errno, e.strerror))
        sys.exit()

    os.chdir('/')
    os.umask(0)
    os.setsid()

    try:
        pid = os.fork()
        if pid > 0:
            sys.exit(0)
    except OSError, e:
        logger.error("fork #2 fail, errno:{0} {1}".format(e.errno, e.strerror))
        sys.exit()

    for f in sys.stdout, sys.stderr:
        f.flush()

    si = open(stdin, 'r')
    so = open(stdout, 'a+')
    se = open(stderr, 'a+', 0)
    os.dup2(si.fileno(), sys.stdin.fileno())
    os.dup2(so.fileno(), sys.stdout.fileno())
    os.dup2(se.fileno(), sys.stderr.fileno())


class ClsSingleton(type):

    """it can be used as meta class"""

    def __init__(cls, name, bases, dict):
        super(ClsSingleton, cls).__init__(name, bases, dict)
        cls._instance = None

    def __call__(cls, *args, **kwargs):
        if cls._instance is None:
            cls._instance = super(ClsSingleton, cls).__call__(*args, **kwargs)
        return cls._instance


# use meta class instread
# @singleton
class HttpDeamon(object):

    __metaclass__ = ClsSingleton

    def __init__(self, ip, port, threadnum=100):
        self.ip = ip
        self.port = port
        self.threadnum = threadnum
        self.tp = LocalThreadPool(self.threadnum)
        self.id = 0
        self.server_socket = self.startup(self.ip, self.port)

        # bug fix, cannot use local startup, why?
        # self.server_socket = self.startup()

    def epoll_run(self):
        """async version, single epoll, then multi thread worker"""
        # self.server_socket = self.startup(self.ip, self.port)

        try:
            # epoll_fd = select.epoll()
            # note: maybe use select, not epoll
            epoll_fd = reactor.Reactor()
            # epoll_fd.register(self.server_socket.fileno(), select.EPOLLIN)
            epoll_fd.register(
                self.server_socket.fileno(), select.EPOLLIN | select.EPOLLERR | select.EPOLLHUP)
        except select.error, msg:
            exit_error(msg)

        connections = {}
        params = {}

        def close_fd(fd):
            logger.debug("{0}:{1} closed".format(
                params[fd]['ip'], params[fd]['port']))
            epoll_fd.unregister(fd)
            connections[fd].close()
            del params[fd]
        
        def reset_fd(fd):
            logger.debug("{0}:{1} reset".format(
                params[fd]['ip'], params[fd]['port']))
            params['time'] = time.time()
            params[fd]['in'] = None
            params[fd]['out'] = None
            epoll_fd.modify(fd, select.EPOLLIN) 
            
        last_time = -1
        while True:
            epoll_list = epoll_fd.poll()

            for fd, events in epoll_list:
                if fd == self.server_socket.fileno():
                    conn, (ip, port) = self.server_socket.accept()
                    logger.debug(
                        "{0}:{1} connected fd={2}".format(ip, port, conn.fileno()))
                    conn.setblocking(0)
                    # epoll_fd.register(conn.fileno(),
                    # select.EPOLLIN|select.EPOLLET)
                    epoll_fd.register(conn.fileno(), select.EPOLLIN)
                    connections[conn.fileno()] = conn
                    params[conn.fileno()] = {
                        'in': None, 'out': None, 'time': time.time(), 'ip': ip, 'port': port}
                elif select.EPOLLIN & events:
                    datas = ''
                    while True:
                        try:
                            data = connections[fd].recv(10)
                            if not data:
                                close_fd(fd)
                                break
                            else:
                                datas += data
                        except socket.error, msg:
                            if msg.errno == errno.EAGAIN:
                                logger.debug("{0}".format(datas))
                                params[fd]["in"] = datas
                                params[fd]["time"] = time.time()
                                # epoll_fd.modify(fd,
                                # select.EPOLLOUT|select.EPOLLET)
                                epoll_fd.modify(fd, select.EPOLLOUT)
                                self.tp.add_job(params[fd])
                                self.id += 1
                                break
                            else:
                                close_fd(fd)
                                logger.error(msg)
                                break
                elif select.EPOLLHUP & events:
                    close_fd(fd)
                elif select.EPOLLOUT & events:
                    data = params[fd].get('out', None)
                    if data is None:
                        continue
                    data_len = len(data)
                    trans_len = 0
                    while True:
                        trans_len += connections[fd].send(data)
                        params[fd]['time'] = time.time()
                        if trans_len == data_len:
                            close_fd(fd)
                            #reset_fd(fd)
                            break
                        # for http protocol, close it direct
                    # epoll_fd.modify(fd, select.EPOLLIN|select.EPOLLET)
                else:
                    continue

            # close unactive sockets, 10 mins!
            cur_time = time.time()
            if cur_time - last_time > 600:
                last_time = time.time()
                for fd, param in params.iteritems():
                    if cur_time - params[fd]["time"] > 600:
                        close_fd(fd)

        epoll_fd.close()

    def __del__(self):
        self.server_socket.close()

    def get_ip(self, ifname):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        return socket.inet_ntoa(fcntl.ioctl(
            s.fileno(),  0x8915, struct.pack('256s', ifname[:15]))[20:24])

    def startup(self, ip, port):
        try:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error, msg:
            self.exit_error("create socket fail")
        try:
            s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        except socket.error, msg:
            self.exit_error("setsockopt error")
        
        try:
            flags = fcntl.fcntl(s, fcntl.F_GETFD)
            fcntl.fcntl(s, fcntl.F_SETFD, flags|fcntl.FD_CLOEXEC)
        except socket.error, msg:
            self.exit_error("fcntl failed")
        
        try:
            flags = fcntl.fcntl(s, fcntl.F_GETFL)
            fcntl.fcntl(s, fcntl.F_SETFL, flags|os.O_NONBLOCK)
        except socket.error, msg:
            self.exit_error("set nonblock failed.")

        try:
            s.bind((ip, port))
        except socket.error, msg:
            self.exit_error("listen file id bind ip fail")
        try:
            s.listen(5)
            s.setblocking(0)
        except socket.error, msg:
            self.exit_error(msg)
        logger.info('httpd running on {0}:{1}'.format(ip, port))
        return s

    def exit_error(self, msg):
        logger.error(msg)
        sys.exit(-1)


class LocalThreadPool(object):

    def __init__(self, num_of_threads=10):
        self.workQueue = Queue.Queue()
        self.resultQueue = Queue.Queue()
        self.threads = []
        self.__createThreadPool(num_of_threads)

    def __createThreadPool(self, num_of_threads):
        for i in range(num_of_threads):
            thread = LocalThread(self.workQueue, self.resultQueue)
            self.threads.append(thread)

    def wait_for_complete(self):
        # 等待所有线程完成。
        while len(self.threads):
            thread = self.threads.pop()
            # 等待线程结束
            if thread.isAlive():  # 判断线程是否还存活来决定是否调用join
                thread.join()

    def add_jobv(self, callable, *args, **kwargs):
        self.workQueue.put((callable, args, kwargs))

    def add_job(self, datas):
        self.workQueue.put(datas)


class LocalThread(threading.Thread):

    def __init__(self, workQueue, resultQueue, timeout=30, **kwargs):
        threading.Thread.__init__(self, kwargs=kwargs)
        # 线程在结束前等待任务队列多长时间
        self.timeout = timeout
        self.setDaemon(True)
        self.workQueue = workQueue
        self.resultQueue = resultQueue
        self.worker = ClsWorker()
        self.start()

    def run(self):
        while True:
            try:
                datas = self.workQueue.get()
                self.worker.run(datas)
            except Queue.Empty:  # 任务队列空的时候结束此线程
                time.sleep(1)
                # break
            except:
                print sys.exc_info()
                raise


@singleton
class ClsHttpUtility(object):
    _100 = "HTTP/1.0 100 CONTINUE"
    _101 = "HTTP/1.0 101 SWITCHING PROTOCOL"
    _102 = "HTTP/1.0 102 PROCESSING"

    _200 = "HTTP/1.0 200 OK"
    _201 = "HTTP/1.0 201 CREATED"
    _202 = "HTTP/1.0 202 ACCEPTED"
    _203 = "HTTP/1.0 203 NON-AUTHORIATIVE INFORMATION"
    _204 = "HTTP/1.0 204 NO CONETENT"
    _205 = "HTTP/1.0 205 RESET CONTENT"
    _206 = "HTTP/1.0 206 PARTIAL CONTENT"
    _207 = "HTTP/1.0 207 MULTI-STATUS"

    _300 = "HTTP/1.0 300 MULTIPLE CHOICES"
    _301 = "HTTP/1.0 301 MOVED PERMANENTLY"
    _302 = "HTTP/1.0 302 FOUND"
    _303 = "HTTP/1.0 303 SEE OTHER"
    _304 = "HTTP/1.0 304 NOT MODIFIED"
    _305 = "HTTP/1.0 305 USE PROXY"
    _307 = "HTTP/1.0 307 TEMPORARY REDIRECT"

    _400 = "HTTP/1.0 400 BAD REQUEST"
    _401 = "HTTP/1.0 401 UNAUTHORIZED"
    _402 = "HTTP/1.0 402 PAYMENT GRANTED"
    _403 = "HTTP/1.0 403 FORBIDDEN"
    _404 = "HTTP/1.0 404 FILE NOT FOUND"
    _405 = "HTTP/1.0 405 METHOD NOT ALLOWED"
    _406 = "HTTP/1.0 406 NOT ACCEPTABLE"
    _407 = "HTTP/1.0 407 PROXY AUTHENTICATION REQUIRED"
    _408 = "HTTP/1.0 408 REQUEST TIME-OUT"
    _409 = "HTTP/1.0 409 CONFLICT"
    _410 = "HTTP/1.0 410 GONE"
    _411 = "HTTP/1.0 411 LENGTH REQUIRED"
    _412 = "HTTP/1.0 412 PRECONDITION FAILED"
    _413 = "HTTP/1.0 413 REQUEST ENTITY TOO LARGE"
    _414 = "HTTP/1.0 414 REQUEST-URI TOO LARGE"
    _415 = "HTTP/1.0 415 UNSUPPORTED MEDIA TYPE"
    _416 = "HTTP/1.0 416 REQUESTED RANGE NOT SATISFIABLE"
    _417 = "HTTP/1.0 417 EXPECTATION FAILED"
    _422 = "HTTP/1.0 422 UNPROCESSABLE ENTITY"
    _423 = "HTTP/1.0 423 LOCKED"
    _424 = "HTTP/1.0 424 FAILED DEPENDENCY"

    _500 = "HTTP/1.0 500 INTERNAL SERVER ERROR"
    _501 = "HTTP/1.0 501 NOT IMPLEMENTED"
    _502 = "HTTP/1.0 502 BAD GATEWAY"
    _503 = "HTTP/1.0 503 SERVICE UNAVAILABLE"
    _504 = "HTTP/1.0 504 GATEWAY TIMEOUT"
    _505 = "HTTP/1.0 505 HTTP VERSION NOT SUPPORTED"
    _506 = "HTTP/1.0 506 INSUFFICIENT STORAGE"

    head = "{0}\r\n\
Server:tinyhttp/01\r\n\
Content-type:text/html\r\n\
\r\n\
"

    template = head + "<html>\r\n\
<head><title>{1}</title></head>\r\n\
<body>{2}</body>\r\n\
</html>"


class ClsWorker(object):

    """async version worker"""
    if not mimetypes.inited:
        mimetypes.init()
    extensions_map = mimetypes.types_map.copy()
    extensions_map.update({
        "": "application/octet-stream",
            ".py": "text/plain",
            ".c": "text/plain",
            ".h": "text/plain",
    })

    weekdayname = ["Mon", "Tue", "Web", "Thu", "Fri", "Sat", "Sun"]
    monthname = [None,
                 "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                 "Jul", "Aug", "Sep", "Oct", "Nov", "Dev"]


    responses = {
        100: ('Continue', 'Request received, please continue'),
        101: ('Switching Protocols',
              'Switching to new protocol; obey Upgrade header'),

        200: ('OK', 'Request fulfilled, document follows'),
        201: ('Created', 'Document created, URL follows'),
        202: ('Accepted',
              'Request accepted, processing continues off-line'),
        203: ('Non-Authoritative Information', 'Request fulfilled from cache'),
        204: ('No Content', 'Request fulfilled, nothing follows'),
        205: ('Reset Content', 'Clear input form for further input.'),
        206: ('Partial Content', 'Partial content follows.'),

        300: ('Multiple Choices',
              'Object has several resources -- see URI list'),
        301: ('Moved Permanently', 'Object moved permanently -- see URI list'),
        302: ('Found', 'Object moved temporarily -- see URI list'),
        303: ('See Other', 'Object moved -- see Method and URL list'),
        304: ('Not Modified',
              'Document has not changed since given time'),
        305: ('Use Proxy',
              'You must use proxy specified in Location to access this '
              'resource.'),
        307: ('Temporary Redirect',
              'Object moved temporarily -- see URI list'),

        400: ('Bad Request',
              'Bad request syntax or unsupported method'),
        401: ('Unauthorized',
              'No permission -- see authorization schemes'),
        402: ('Payment Required',
              'No payment -- see charging schemes'),
        403: ('Forbidden',
              'Request forbidden -- authorization will not help'),
        404: ('Not Found', 'Nothing matches the given URI'),
        405: ('Method Not Allowed',
              'Specified method is invalid for this resource.'),
        406: ('Not Acceptable', 'URI not available in preferred format.'),
        407: ('Proxy Authentication Required', 'You must authenticate with '
              'this proxy before proceeding.'),
        408: ('Request Timeout', 'Request timed out; try again later.'),
        409: ('Conflict', 'Request conflict.'),
        410: ('Gone',
              'URI no longer exists and has been permanently removed.'),
        411: ('Length Required', 'Client must specify Content-Length.'),
        412: ('Precondition Failed', 'Precondition in headers is false.'),
        413: ('Request Entity Too Large', 'Entity is too large.'),
        414: ('Request-URI Too Long', 'URI is too long.'),
        415: ('Unsupported Media Type', 'Entity body in unsupported format.'),
        416: ('Requested Range Not Satisfiable',
              'Cannot satisfy request range.'),
        417: ('Expectation Failed',
              'Expect condition could not be satisfied.'),

        500: ('Internal Server Error', 'Server got itself in trouble'),
        501: ('Not Implemented',
              'Server does not support this operation'),
        502: ('Bad Gateway', 'Invalid responses from another server/proxy.'),
        503: ('Service Unavailable',
              'The server cannot process the request due to a high load'),
        504: ('Gateway Timeout',
              'The gateway server did not receive a timely response'),
        505: ('HTTP Version Not Supported', 'Cannot fulfill request.'),
        }


    def __init__(self):
        self.utility = ClsHttpUtility()

    def run(self, datas):
        """
        :param datas: datas={'in':, 'out':,}
        """
        logger.debug("{0}:{1} worker begin".format(
            datas['ip'], datas['port']))

        arr = datas['in'].split('\n')
        method, url, version = arr[0].split()

        gzip = False
        for line in arr[1:]:
            if line.find('Accept-Encoding') != -1:
                if line.find('gzip') != -1:
                    gzip = True

        request_path = url.split('?')[0]

        datas['out'] = self.serve_file(request_path, gzip)

        logger.debug("{0}:{1} worker end".format(
            datas['ip'], datas['port']))

    def serve_file(self, request_path, compress=False):
        if not os.path.exists(request_path):
            request_path = '{0}/{1}'.format(
                FILEPATH, request_path)

        if not os.path.isfile(request_path):
            return self.utility.template.format(
                self.utility._404,
                request_path,
                "File not Found")

        if not os.access(request_path, os.R_OK):
            return self.utility.template.format(
                self.utility._403,
                request_path,
                "Permission deny")

        with open(request_path, 'r') as f:
            contents = f.read()

        filetype = request_path.split('.')[-1]
        if compress:
            buf = StringIO.StringIO()
            f = gzip.GzipFile(mode='wb', fileobj=buf)
            
            #if filetype == "html":
            #    f.write(contents)
            #else:
            #    f.write("<html><head><title>{1}</title></head><body><pre>{1}</pre></body></html>".format(
            #        request_path,
            #        contents))

            f.write(contents) 

            f.close()
            contents = buf.getvalue()
            result = "{}\r\n\
Date: {}\r\n\
Server: tinyhttpd/0.1\r\n\
Content-Encoding:gzip\r\n\
Content-Type: {}\r\n\
Connection: close\r\n\
\r\n\
{}\
\r\n".format(self.utility._200,
             self.date_time_string(),
             self.guess_type(request_path),
             contents)
        else:
            result = "{}\r\n\
Date: {}\r\n\
Server: tinyhttpd/0.1\r\n\
Content-Encoding:gzip\r\n\
Content-Type: {}\r\n\
Connection: close\r\n\
\r\n\
{}\
\r\n".format(self.utility._200,
             self.date_time_string(),
             self.guess_type(request_path),
             contents)
        return result

    def guess_type(self, path):
        """more accurate, use it later"""
        base, ext = posixpath.splitext(path)
        if ext in self.extensions_map:
            return self.extensions_map[ext]
        if ext.lower() in self.extensions_map:
            return self.extensions_map[ext]
        else:
            return self.extensions_map[ext]

    def date_time_string(self, timestamp=None):
        if timestamp is None:
            timestamp = time.time()
        year, month, day, hh, mm, ss, wd, y, z = time.gmtime(timestamp)
        # s = "%02d/%3s/%04d %02d:%02d:%02d"%(
        #        day, self.monthname[month], year, hh, mm, ss)

        s = "%s, %02d %3s %4d %02d:%02d:%02d GMT" % (
            self.weekdayname[wd],
            day, self.monthname[month], year,
            hh, mm, ss)
        return s

class ForkingMixIn:
    timeout = 300
    active_children = None
    max_children = 40

    def collection_child(self):
        if self.active_children is None:
            return
        while len(self.active_children) >= self.max_children:
            try:
                pid, status = os.waitpid(0, 0)
            except os.error:
                pid = None
            if pid not in self.active_children:
                continue

        for child in self.active_children:
            try:
                pid, status = os.waitpid(child, os.WHOHANG)
            except os.error:
                pid = None
            if not pid: continue
            try:
                self.active_children.remove(pid)
            except ValueError, e:
                raise("%s x=%d and list=%r"% (e.message, pid,
                                              self.active_children))
    def handle_timeout(self):
        self.collection_child()

    def process_request(self, request, client_address):
        self.collection_child()
        pid = os.fork()
        if pid:
            if self.active_children is None:
                self.active_children = []
            self.active_children.append(pid)
            return
        else:
            try:
                #to be continue
                pass
            except:
                pass

if __name__ == '__main__':
    args = parse_argument()
    ip, port = args.ip, args.port
    logger = config_log()
    # daemonize()
    inst = HttpDeamon(ip, port)
    inst.epoll_run()
