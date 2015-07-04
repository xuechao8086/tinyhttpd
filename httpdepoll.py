#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       httpd.py
Create Time:    2015-07-01 23:37
Description:
    python version tiny httpd for study purpose
ToDo:
    implement the httpd protocol.
Memo:
    pep8
    autopep8 check.
    yapf check.
"""

import argparse
import os
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
import errno

from threadpool import ThreadPool

FILEPATH = '/home/charlie/tinyhttpd/'
GET = 'GET'
POST = 'POST'
HEAD = 'HEAD'


def config_log():
    logger = logging.getLogger('out')
    logger.setLevel(logging.DEBUG)

    fh = logging.FileHandler("./httpd.log")
    fh.setLevel(logging.INFO)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

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


def daemonize(stdin='/dev/null', stdout='/dev/null', stderr='/dev/null'):
    """daemonize the process, no need to start the process in nohup way"""
    try:
        pid = os.fork()
        if pid > 0:
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


class HttpDeamon(object):

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
            epoll_fd = select.epoll()
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
                            if not data and not datas:
                                close_fd(fd)
                                break
                            else:
                                datas += data
                        except socket.error, msg:
                            if msg.errno == errno.EAGAIN:
                                logger.debug("{0}".format(datas))
                                params[fd]['in'] = datas
                                params[fd]['time'] = time.time()
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
                            break
                        # for http protocol, close it direct
                    close_fd(fd)
                    # epoll_fd.modify(fd, select.EPOLLIN|select.EPOLLET)
                else:
                    continue

            # close unactive sockets, 10 mins!
            cur_time = time.time()
            if cur_time - last_time > 600:
                for fd, param in params.iteritems():
                    if cur_time - param['time'] > 600:
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
        self.worker = EpollWorker()
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


class EpollWorker(object):

    """async version worker"""

    def __init__(self):
        pass

    def run(self, datas):
        """
        :param datas: format datas={'in':, 'out':,}
        """
        logger.debug("{0}:{1} worker begin".format(
            datas['ip'], datas['port']))

        arr = datas['in'].split('\n')
        method, url, version = arr[0].split()

        request_path = '{0}/{1}'.format(
                       FILEPATH, url.split('?')[0])

        with open(request_path, 'r') as f:
            contents = f.read()
        datas['out'] = """
                    HTTP/1.0 200 OK\r\n
                    Content-Type: text/html\r\n
                    \r\n
                    <html>
                    <head><title>{0}</title></head>
                    <body><pre>{1}</pre></body>
                    </html>
                    \r\n
                    """.format(request_path, contents)

        logger.debug("{0}:{1} worker end".format(
            datas['ip'], datas['port']))
        # import pprint
        # print 'datas = '
        # pprint.pprint(datas)


if __name__ == '__main__':
    args = parse_argument()
    ip, port = args.ip, args.port
    logger = config_log()
    # daemonize()
    HttpDeamon(ip, port).epoll_run()
