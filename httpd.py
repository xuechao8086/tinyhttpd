#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       httpd.py
Create Time:    2015-07-01 23:37
Description:
    python version tiny httpd for study purpose               
ToDo:
    fix bug in HttpDeamon.startup    
"""

import argparse
import os
import stat
import socket
import fcntl
import struct
import time

import Queue
import threading
import sys
import time
import logging

from threadpool import ThreadPool

FILEPATH = '/home/charlie/tinyhttpd/'
GET = 'GET'
POST = 'POST'
HEAD = 'HEAD'

def config_log():
    logger = logging.getLogger('out')
    logger.setLevel(logging.DEBUG)

    ch = logging.StreamHandler()
    ch.setLevel(logging.DEBUG)

    formatter = logging.Formatter('%(asctime)s - %(levelname)s - %(message)s')
    ch.setFormatter(formatter)
    
    logger.addHandler(ch)

    return logger

def parse_argument():
    parser = argparse.ArgumentParser(
            description = 'tiny httpd server, written in python',
            epilog = 'example: %(prog)s')

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
    def __init__(self, ip, port, threadnum=1000):
        self.ip = ip
        self.port = port
        self.threadnum = threadnum
        self.tp = ThreadPool(self.threadnum)
        self.id = 0 
        self.server_socket = startup(self.ip, self.port)
        
        #bug fix, cannot use local startup, why?
        #self.server_socket = self.startup()
    
    def run(self):
        while True:
            client_socket, (ip, port) = self.server_socket.accept()
            logger.debug('receive request from {0}:{1}'.format(ip, port))
            self.tp.add_job(task, self.id, client_socket)
            self.id += 1

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


def startup(ip, port):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((ip, port))
    s.listen(5)
    logger.info('httpd running on {0}:{1}'.format(ip, port))
    return s


def task(args, kwargs):
    id, client_socket = args 
    return JobCls(client_socket).accept_request()


class JobCls(object):
    def __init__(self, client_socket):
        self.client_socket = client_socket

    def __del__(self):
        self.client_socket.close()

    def accept_request(self):
        client_content = ''
        while True:
            client_content = self.client_socket.recv(1024)
            break
        client_content = client_content.strip()
        
        print client_content
        arr = client_content.split('\n')
        method, url, version = arr[0].split()
        method = method.upper()
        #self.client_socket.sendall('begin process')
        if method not in (GET, POST, HEAD):
            return self.server_error(self.client_socket)
        
        cgi = False
        query_string = '' 
        pos = 0
        r_path = url
        if method == POST:
            cgi = True 
        elif method == GET:
            pos = url.find('?')
            if pos != -1:
                cgi = True
                query_string = url[pos:]
                r_path  = url[0:pos]
    
    
        r_path = '{0}/{1}'.format(FILEPATH, r_path)
        if cgi:
            self.execute_cgi(r_path)
        else:
            self.serve_file(r_path)
        
        return str(id)
    
    
    def execute_cgi(self, r_path):
        self.client_socket.send(r_path)
    
    def serve_file(self, filename):
        if not os.path.isfile(filename): 
            return self.server_error(self.client_socket)
        if not os.access(filename, os.R_OK):
            return self.server_error(self.client_socket)
        self.headers()
        with open(filename, 'r') as f:
            self.client_socket.send(
                   """
                <html>
                   <head>
                    <title>{0}</title> 
                   </head>
                   <body>
                    <pre>{1}</pre>
                   </body>
                </html>""".format(filename, f.read())
                )
    
    def headers(self):
        headers = [
                    "HTTP/1.0 200 OK\r\n",
                    "Content-Type: text/html\r\n",
                    "\r\n"
                   ]
        for i in headers:
            self.client_socket.send(i)
    
    def not_found(self):
        contents = [
                    "HTTP/1.0 404 NOT FOUND\r\n",
                    "Content-Type: text/html\r\n",
                    "\r\n",
                    "<html>",
                    "<title>404 not found</title>",
                    "<body><p>the content you search for not found on the server</body>",
                    "</html>"
                    ]
        for i in contents:
            self.client_socket.send(i)
    
    
    def server_error(self):
        contents = [
                    "HTTP/1.0 500 Internal Server Error\r\n",
                    "Content-Type: text/html\r\n",
                    "\r\n",
                    "<html>",
                    "<title>505 server error</title>",
                    "<body><p>server error</p></body>",
                    "</html>"
                    ]
        for i in contents:
            self.client_socket.send(i)
   

if __name__ == '__main__':
    logger = config_log()
    args = parse_argument()
    ip, port = args.ip, args.port
    HttpDeamon(ip, port).run()
