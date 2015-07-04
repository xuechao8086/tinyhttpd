#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       httpd.py
Create Time:    2015-07-01 23:37
Description:
    python version tiny httpd for study purpose               
ToDo:
    fix bug in HttpDeamon.startup.    
    add epoll close unactive sockets.
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
    ch.setFormatter(formatter)
    fh.setFormatter(formatter) 

    logger.addHandler(ch)

    return logger

def parse_argument():
    parser = argparse.ArgumentParser(
            description = 'tiny httpd server, written in python',
            epilog = 'example: %(prog)s -i 127.0.0.1 -p 8080 ')

    parser.add_argument('-i', '--ip', 
                        metavar='ip',
                        #nargs=1,
                        type=str, default='127.0.0.1',
                        help='ip address')
    
    parser.add_argument('-p', '--port',
                        metavar='port', 
                        #nargs=1,
                        type=int, default=8080,
                        help='port')

    return parser.parse_args() 


class HttpDeamon(object):
    def __init__(self, ip, port, threadnum=100):
        self.ip = ip
        self.port = port
        self.threadnum = threadnum
        self.tp = LocalThreadPool(self.threadnum)
        self.id = 0 
        self.server_socket = startup(self.ip, self.port)
        
        #bug fix, cannot use local startup, why?
        #self.server_socket = self.startup()
    
    def epoll_run(self):
        """async version, single epoll, then multi thread worker"""
        try:
            epoll_fd = select.epoll()
            #epoll_fd.register(self.server_socket.fileno(), select.EPOLLIN)
            epoll_fd.register(self.server_socket.fileno(), select.EPOLLIN|select.EPOLLERR|select.EPOLLHUP)
        except select.error, msg:
            exit_error(msg)
        
        connections = {}
        addresses = {}
        params = {}

        def close_fd(fd):
            logger.debug("{0}:{1} closed".format(addresses[fd][0], addresses[fd][1]))
            epoll_fd.unregister(fd)
            connections[fd].close()
            del addresses[fd]
            del params[fd]
        
        last_time = -1
        while True:
            epoll_list = epoll_fd.poll()
            for fd, events in epoll_list:
                if fd == self.server_socket.fileno():
                    conn, addr = self.server_socket.accept()
                    logger.debug("accept connection from {0}:{1} fd={2}".format(addr[0], addr[1], conn.fileno()))
                    conn.setblocking(0)
                    epoll_fd.register(conn.fileno(), select.EPOLLIN|select.EPOLLET) 
                    connections[conn.fileno()] = conn
                    addresses[conn.fileno()] = addr
                    params[conn.fileno()] = {'in':None, 'out':None, 'time':time.time()}
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
                                logger.debug("{0} receive {1}".format(fd, datas))
                                params[fd]['in'] = datas 
                                params[fd]['time'] = time.time()
                                epoll_fd.modify(fd, select.EPOLLET|select.EPOLLOUT)
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
                    if data == None:
                        continue
                    data_len = len(data)
                    trans_len = 0
                    while True:
                        trans_len += connections[fd].send(data)
                        params[fd]['time'] = time.time()
                        if trans_len == data_len:
                            break
                        #for http protocol, close it direct
                    close_fd(fd) 
                    #epoll_fd.modify(fd, select.EPOLLIN|select.EPOLLET)
                else:
                    continue

            #close unactive sockets, 10 mins!
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

    def startup(self):
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.bind((self.ip, self.port))
        s.listen(5)
        logger.info('httpd running on {0}:{1}'.format(self.ip, self.port))
        return s

def exit_error(msg):
    logger.error(msg)
    sys.exit(-1)

def startup(ip, port):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error, msg:
       exit_error("create socket fail")
    try:    
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    except socket.error, msg:
        exit_error("setsockopt error")
    try:
        s.bind((ip, port))
    except socket.error, msg:
        exit_error("listen file id bind ip fail")
    try:
        s.listen(5)
        s.setblocking(0)
    except socket.error, msg:
        exit_error(msg)
    logger.info('httpd running on {0}:{1}'.format(ip, port))
    return s


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
                #break
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
        
        #import pprint
        #print 'datas = '
        #pprint.pprint(datas)

if __name__ == '__main__':
    logger = config_log()
    args = parse_argument()
    ip, port = args.ip, args.port
    #HttpDeamon(ip, port).run()
    HttpDeamon(ip, port).epoll_run()
