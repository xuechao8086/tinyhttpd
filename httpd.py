#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       httpd.py
Create Time:    2015-07-01 23:37
Description:
    python version tiny httpd for study purpose               
"""
import os
import stat
import socket
import fcntl
import struct
import time

FILEPATH = '/home/charlie/tinyhttpd/'
PORT = 9734
GET = 'GET'
POST = 'POST'
HEAD = 'HEAD'

def get_ip(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),  0x8915, struct.pack('256s', ifname[:15]))[20:24])


def startup():
    host = get_ip('eth0')
    port = PORT 
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.bind((host, port))
    s.listen(5)
    print 'httpd running on port {0}'.format(PORT)
    return s


def main():
    server_socket = startup()
    while True:
        client_socket, addr = server_socket.accept()
        time.sleep(0.01)
        print '{0} {1}'.format(client_socket, addr)
        accept_request(client_socket) 
        client_socket.close() 
    
    server_socket.close()

def accept_request(client_socket):
    client_content = ''
    while True:
        client_content = client_socket.recv(1024)
        break
    client_content = client_content.strip()
    
    print client_content
    arr = client_content.split('\n')
    method, url, version = arr[0].split()
    method = method.upper()
    #client_socket.sendall('begin process')
    if method not in (GET, POST, HEAD):
        return server_error(client_socket)
    
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
        execute_cgi(client_socket, r_path)
    else:
        serve_file(client_socket, r_path)

def execute_cgi(client_socket, r_path):
    client_socket.send(r_path)


def serve_file(client_socket,  filename):
    if not os.path.isfile(filename): 
        return server_error(client_socket)
    if not os.access(filename, os.R_OK):
        return server_error(client_socket)
    headers(client_socket)
    with open(filename, 'r') as f:
        client_socket.send(
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


def headers(client_socket):
    headers = [
                "HTTP/1.0 200 OK\r\n",
                "Content-Type: text/html\r\n",
                "\r\n"
               ]
    for i in headers:
        client_socket.send(i)


def not_found(client_socket):
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
        client_socket.send(i)


def server_error(client_socket):
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
        client_socket.send(i)


def test():
    print get_ip('eth0')


if __name__ == '__main__':
    #start here....
    #test()
    main()     
