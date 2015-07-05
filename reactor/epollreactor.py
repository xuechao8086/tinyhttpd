#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       epollreactor.py
Create Time:    2015-07-05 05:54
Description:
                
"""
    
from select import epoll, EPOLLHUP, EPOLLERR, EPOLLIN, EPOLLOUT, EPOLLET

class EpollReactor(object):
    EV_CONNECTED = (EPOLLHUP | EPOLLERR)
    EV_IN = EPOLLIN
    EV_OUT = EPOLLOUT
    
    def __init__(self):
        self._poller = epoll()

    def poll(self, timeout=-1):
        return self._poller.poll(timeout)
    
    def register(self, fd, mode):
        self._poller.register(fd, mode)
    
    def modify(self, fd, mode):
        self._poller.modify(fd, mode)

    def unregister(self, fd):
        self._poller.unregister(fd)
    
    def close(self, fd):
        self._poller.close()

if __name__ == '__main__':
    pass
     
     
