#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       4.5.py
Create Time:    2015-10-20 00:17
Description:
                
"""
import sys
import collections

content = map(lambda key: key.strip(), sys.stdin.read().split())
cnt = collections.Counter()

def update(key):
    global cnt
    cnt[key] += 1

map(update, content)

print(filter(lambda key: cnt[key]>1, cnt))

if __name__ == '__main__':
    pass
