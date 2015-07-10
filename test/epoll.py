#!/usr/bin/env python
#encoding:utf-8  
  
import os  
import sys  
import socket  
import select  
import time  
  
NUM = 3  
  
def process_req(serversocket, pos):  
    pid = os.getpid()  
    epoll = select.epoll()  
  
    held = 0  
    connections = {}  
    requests = {}  
    responses = {}  
  
    while True:  
        minutes = time.localtime()[4]  
        if minutes % NUM == pos:  
            if held==0:  
                epoll.register(serversocket.fileno(), select.EPOLLIN)  
                held = 1  
        elif held == 1:  
            epoll.unregister(serversocket.fileno())  
            held = 0  
  
        events = epoll.poll(1)  
        for fileno, event in events:  
            if fileno == serversocket.fileno():  
                connection, address = serversocket.accept()  
                connection.setblocking(0)  
                epoll.register(connection.fileno(), select.EPOLLIN)  
  
                connections[connection.fileno()] = connection  
                requests[connection.fileno()] = ''  
                responses[connection.fileno()] = ''  
                print 'accept socket, pid = %d, fileno= %d' % (pid, fileno)  
  
            elif event & select.EPOLLIN:  
                requests[fileno] += connections[fileno].recv(1024)  
                if requests[fileno].find('END') >= 0:  
                    responses[fileno] = requests[fileno]  
                    epoll.modify(fileno, select.EPOLLOUT)  
                    print requests[fileno]  
            elif event & select.EPOLLOUT:  
                byteswritten = connections[fileno].send(responses[fileno])  
                responses[fileno] = responses[fileno][byteswritten:]  
                if len(responses[fileno]) == 0:  
                    epoll.modify(fileno, 0)  
                    connections[fileno].shutdown(socket.SHUT_RDWR)  
  
            elif event & select.EPOLLHUP:  
                epoll.unregister(fileno)  
                connections[fileno].close()  
                del connections[fileno]  
  
    epoll.unregister(serversocket.fileno())  
    epoll.close()  
    serversocket.close()  
  
  
  
if __name__ == "__main__":  
  
    serversocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  
    serversocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  
    serversocket.bind(('0.0.0.0', 8888))  
    serversocket.listen(1)  
    serversocket.setblocking(0)  
  
    pid_arr = []  
  
    for i in range(0, NUM):  
        pid = os.fork()  
        if pid == 0:  
            print 'in child: ', os.getpid()  
            process_req(serversocket, i)  
            sys.exit()  
        else:  
            pid_arr.append(pid)  
  
    serversocket.close()  
  
    for pid in pid_arr:  
        os.waitpid(pid, 0)  
  
    print '~~~ finish ~~~'  
