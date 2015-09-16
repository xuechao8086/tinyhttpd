#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       kmp.py
Create Time:    2015-09-15 13:19
Description:
    kmp                
"""
from __future__ import print_function


def index(s, t, pos):
    i, j = pos, 0
    len_s = len(s)
    len_t = len(t)
    while i < len_s and j < len_t:
        if s[i] == t[j]:
            i += 1
            j += 1
        else:
            i = i - j + 1;
            j = 0

    if j >= len_t:
        return i - len_t
    else:
        return 0


def index_kmp(s, t, pos):
    i, j = pos, 0
    len_s, len_t = len(s), len(t)

    while i < len_s and j < len_t:
        if j == 0 or s[i] == t[j]:
            i += 1
            j += 1
        else:
            j = next[j]

    if j>= len_t:
        return i - len_t
    else:
        return 0


def getnext(t):
    """from pos=1 begin"""
    next = [ 0 for i in xrange(len(t))]
    i, j = 1, 0
    
    next[1] = 0
    len_t = len(t)
    while i < len_t - 1:
        if j == 0 or t[i] == t[j]:
            i += 1
            j += 1
            next[i] = j
        else:
            j = next[j]
    next.pop(0)
    return next


def getnextval(t):
    next = [ 0 for i in xrange(len(t))]
    i, j = 1, 0
    
    next[1] = 0
    len_t = len(t)
    while i < len_t - 1:
        if j == 0 or t[i] == t[j]:
            i += 1
            j += 1
            next[i] = j
            if t[i] != t[j]:
                next[i] = j
            else:
                next[i] = next[j]
        else:
            j = next[j]
    next.pop(0)
    return next


def getnext2(t):
    """from pos=1 begin, consider how to fix!"""
    next = [ 0 for i in xrange(len(t)+1)]
    i, j = 1, 0
    
    next[1] = 0
    len_t = len(t)
    while i < len_t:
        if t[i] == t[j]:
            i += 1
            j += 1
            next[i] = j
        
        else:
            j = next[j]
    return next

class Test(object):
    def __init__(self, a=None):
        if a != None:
            self.a = a


if __name__ == '__main__':
    print(index("aaabaaaaa", "aaaa", 0))
    print(index("aaabaaaaa", "aaaa", 1))
    print(index("-a-ab-abc-abcd", "abc", 1))

    print(getnext(" aaaab"))
    print(getnextval("aaabbb"))



#for test
    a = Test()
    if getattr(a, 'a') != None:
        print("getattr success")
