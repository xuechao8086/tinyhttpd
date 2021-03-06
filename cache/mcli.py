#!/usr/bin/env python2.7
#coding=utf-8

"""
Author:         charliezhao
EMail:          xuechao8086@126.com
Filename:       mcli.py
Create Time:    2015-12-04 21:37
Description:
                
"""
import time
import os
import random
import socket
import sys
import time

class MemcacheTestCli(object):
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port 
    
    def test_long(self):
        self.s = socket.socket(socket.AF_INET,
                               socket.SOCK_STREAM) 
        self.s.connect((self.ip, self.port))
        while True:
            key = "k{}".format(random.randint(1, 19860903))
            value = "v{}".format(random.randint(1, 19860903))
            cmd = "set {} {}\n".format(key, value)
            
            try:
                self.s.sendall(cmd)
                print(">{}".format(cmd))
                # print(self.s.recv(1024)) 
            except:
                print("\033[31m set fail\033[0m")

            time.sleep(0.5)
    
    def test_long2(self):
        self.s = socket.socket(socket.AF_INET,
                               socket.SOCK_STREAM)
        
        self.s.connect((self.ip, self.port))
        
        while(True): 
            # cmd = "{} {};set {}; get {};".format(
            #       time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),
            #       os.getpid(),
            #       random.randint(1, 100),
            #       random.randint(1, 100))
            
            cmd = "set {}k {}v; get k{};".format(random.randint(1, 100),
                                                 random.randint(1, 100),
                                                 random.randint(1, 100));
            try:
                print(cmd)
                self.s.sendall(cmd)
            except:
                print("\033[31m set fail\033[0m")
            time.sleep(1) 

    def test_long3(self):
        self.s = socket.socket(socket.AF_INET,
                               socket.SOCK_STREAM)
        self.s.connect((self.ip, self.port))
        
        rnd = random.randint(1, 19860903);
        cmd = "set {}k {}v; get {}k; set {}k {}{}------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------v;".format(rnd, rnd, rnd, rnd, rnd, rnd)
        try:
            print(cmd)
            self.s.sendall(cmd)
        except:
            print("\033[31m set fail\033[0m")
        cmd = "get {}k".format(rnd)
        try:
            print(cmd)
            self.s.sendall(cmd)
        except:
            print("\033[31m get fail\033[0m")

    def test_long4(self):
        self.s = socket.socket(socket.AF_INET,
                               socket.SOCK_STREAM)
        self.s.connect((self.ip, self.port))
        
        rnd = random.randint(1, 19860903);
    
        cmd = "set k{} v{};".format(rnd, rnd)
        try:
            print(cmd)
            self.s.sendall(cmd)
            print(self.s.recv(1024))
        except:
            print("\033[31m set fail\033[0m")

        cmd = "get k{};".format(rnd)
        try:
            print(cmd)
            self.s.sendall(cmd)
            print(self.s.recv(1024))
        except:
            print("\033[31m set fail\033[0m")

    def test_short(self):
        while True:
            s = socket.socket(socket.AF_INET,
                              socket.SOCK_STREAM)
            s.connect((self.ip, self.port))
            key = "k{}".format(random.randint(1, 19860903))
            value = "v{}".format(random.randint(1, 19860903))
            cmd = "set {} {}\n".format(key, value)
            
            try:
                s.sendall(cmd)
                print(s.recv(1024))
            except:
                print("\033[31m set fail\033[0m")
            time.sleep(1)

def usage():
    print("""{} [1-9]
      1 test_long 
      2 test_short
      3 test_long2
      4 test_long3
      5 test_long4""".format(sys.argv[0]))
    sys.exit()    


if __name__ == '__main__':
    if len(sys.argv) == 1:
        usage()
    m = MemcacheTestCli('localhost', 11211)
    if sys.argv[1] == '1':
        m.test_long()
    elif sys.argv[1] == '2':
        m.test_short()
    elif sys.argv[1] == '3':
        m.test_long2()
    elif sys.argv[1] == '4':
        m.test_long3()
    elif sys.argv[1] == '5':
        m.test_long4()
    else:
        usage()
