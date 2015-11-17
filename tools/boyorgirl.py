#!/usr/bin/env python2.7
#coding=utf8

"""
Author:         charliezhao
Filename:       boyorgirl.py
Create Time:  2015-11-10 05:06
Description:
                
"""
import random
import signal
import sys

__all__ = ['alarm_handle', 'quit_handle', 'int_handle']

result = True

def alarm_handle(signo, frame):
    global result
    if result:
        print 'boy'
    else:
        print 'girl'
    sys.exit(0)    


def quit_handle(signo, frame):
    global result
    if result:
        print 'boy'
    else:
        print 'girl'
    sys.exit(0)


def int_handle(signo, frame):
    quit_handle(signo, frame)


if __name__ == '__main__':
    signal.signal(signal.SIGALRM, alarm_handle)
    signal.signal(signal.SIGQUIT, quit_handle)    
    signal.signal(signal.SIGINT, int_handle)
    signal.alarm(60)
    
    while True:
        choice = random.randint(0, 1<<20)
        if choice & 0x1:
            result = True
        else:
            result = False
