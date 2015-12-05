#!/usr/bin/env python2.7
#coding=utf-8

"""
Author:         charliezhao
EMail:          xuechao8086@126.com
Filename:       mcli.py
Create Time:    2015-12-04 21:37
Description:
                
"""
import random
import socket
import time

class MemcacheTestCli(object):
    def __init__(self, ip, port):
        self.s = socket.socket(socket.AF_INET,
                               socket.SOCK_STREAM) 
    
        self.s.connect((ip, port))

    def test(self):
        
        while True:
            key = "k{}".format(random.randint(1, 19860903))
            value = "v{}".format(random.randint(1, 19860903))
            cmd = "set {} {}\n\0".format(key, value)
            
            try:
                self.s.sendall(cmd)
                print(">{}".format(cmd))
                print(self.s.recv(1024)) 
            except:
                print("\033[31m set fail\033[0m")
                break

            time.sleep(1)


if __name__ == '__main__':
    MemcacheTestCli('localhost', 11211).test()
