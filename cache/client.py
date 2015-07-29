#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       test.py
Create Time:    2015-07-28 06:49
Description:
                
"""
import memcache
import random
import sys
import time

def usage():
    print sys.argv[0], "[-l, for long test]", "[-h help]"
    sys.exit(-1)

if __name__ == '__main__':
    mc = memcache.Client(['127.0.0.1:11211'], debug=True, socket_timeout=3600)
    choice = ("charliezhao", "tingtingzhang", "alisionzhang", 
            "xuechaozhao", "advance programming in UNIX Program", "UNIX network programming")
    if len(sys.argv) == 1: 
        name = choice[int(random.random()*10000)%6] 
        mc.set("name", name)
        print "info: memcache set ok"
        # time.sleep(60)
        value = mc.get("name")
        print "name={}".format(value)
        print "info: memcache get ok"
    elif sys.argv[1] == "-h":
        usage()
    else:
        for i in xrange(10): 
            name = choice[int(random.random()*10000)%6] 
            mc.set("name", name)
            print "name={}".format(mc.get("name"))
