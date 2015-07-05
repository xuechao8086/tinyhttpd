#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       selecteator.py
Create Time:    2015-07-05 06:11
Description:
                
"""
import thread
import threading
import copy

from select import select
from collections import defaultdict


def check_read_ev(ev_fd):
    while True:
        ev_fd._start_read_ev.wait()
        ev_fd._start_read_ev.clear()
        w_list = set()
        x_list = set()
    try:
        r, w, x = select(ev_fd._r_list, w_list, x_list)
    except Exception, e:
        print str(e)
        pass
    
    ev_fd._lock.acquire()
    ev_fd.r = r
    ev_fd._lock.release()
    ev_fd._has_ev.set()


class SelectReactor(object):
    EV_NULL = 0x00
    EV_DISCONNECTED = 0x18
    EV_IN = 0x01
    EV_OUT = 0x04

    def __init__(self):
        self._r_list = set()
        self._w_list = set()
        self._x_list = set()
        self._has_ev = threading.Event()
        self._has_ev.clear()
        self._start_read_ev = threading.Event()
        self._start_read_ev.set()
        self._lock = threading.Lock()
        self.r = set()
        self.w = set()
        self.x = set()
        thread.start_new_thread(check_read_ev, (self,))

    def poll(self, timeout=-1):
        if not self._w_list:
            self._has_ev.wait()
            self._has_ev.clear()

        if self._w_list:
            r_list = set()
            r, w, x = select(r_list, self._w_list, self._x_list)
            self.w = w
            self.x = x

        self._lock.acquire()
        r_list = copy.deepcopy(self.r)
        if r_list:
            self._start_read_ev.set()
        self.r = set()
        self._lock.release()
    
        results = defaultdict(lambda: set.EV_NULL)
        for p in [(r_list, self.EV_IN), (self.w, self.EV_OUT), (self.x, self.EV_DISCONNECTED)]:
            for fd in p[0]:
                results[fd] |= p[1]

        self.w = set()
        self.x = set()

        return results.items()

    def register(self, fd, mode):
        if mode&self.EV_IN:
            self._r_list.add(fd)
        if mode&self.EV_OUT:
            self._w_list.add(fd)
        self._x_list.add(fd)

    def unregister(self, fd):
        self._r_list.discard(fd)
        self._w_list.discard(fd)
        self._x_list.discard(fd)

    def modify(self, fd, mode):
        if mode&self.EV_IN:
            self._r_list.add(fd)
        else:
            self._r_list.discard(fd)
        
        mode_w = False
        if mode&self.EV_OUT:
            self._w_list.add(fd)
            mode_w = True
        else:
            self._w_list.discard(fd)
        self._x_list.add(fd)

        if mod_w:
            self._has_ev.set()























if __name__ == '__main__':
    #start here....
    pass     
     
     
