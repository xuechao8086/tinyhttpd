#!/usr/bin/env python2.7
#coding:utf-8
"""
Author:         charliezhao
Filename:       skiplist.py
Create Time:    2015-09-12 10:05
Description:
    link list                
"""
from __future__ import print_function
import random


class Node(object):
    def __init__(self, val, p):
        self.val = val
        self.next = p


class LinkList(object):
    def __init__(self):
        self.head = None
    
    def __getitem__(self, key):
        """
        `param key`: index
        """
        p = self.head
        i = 0
        while i < key and p:
            p=p.next
            i += 1 
        if i == key:
            return p.val
        else:
            return None

    def __setitem__(self, key, value):
        p = self.head
        i = 0
        while i < key:
            i += 1
            p = p.next
        p.val = value
    
    def __iter__(self):
        p = self.head
        while p:
            yield p.val
            p = p.next 
        
    def __len__(self):
        i = 0
        p = self.head
        while p:
            i += 1
            p = p.next
        return i
    
    def __del__(self):
        print("self.__del__ called")
        p = self.head
        while p:
            s = p.next
            del p
            p = s

    def __enter__(self):
        print("self.__enter__ called")
        
    def __exit__(self, exc_type=None, exc_value=None, traceback=None):
        print("self.__exit__ called")


    def init(self, nodes):
        """
        `param nodes`: [node0, node1, node2]
        """
        self.head = nodes[0]
        p = self.head 
        
        for node in nodes[1:]:
            p.next = node
            p = p.next 
    
    def output(self):
        p = self.head
        i = 0
        while p!= None:
            print("idx={} val={}".format(i, p.val))
            i += 1
            p = p.next
        print("###########################################")

if __name__ == '__main__':
    #start here....
    nodes = [Node(random.randint(1, 100), None) \
             for i in xrange(random.randint(5, 10))]
    i = 0 
    #for node in nodes:
    #    print("{:>3d}".format(node.val))

    ll = LinkList()
    ll.init(nodes)
    ll.output()
        
    print("ll[{}]={}".format(2,ll[2]))
    ll[2] = 1998
    print("ll[{}]={}".format(2,ll[2]))
    print("len(ll)={}".format(len(ll)))
    print("###########################################")
    with LinkList() as ll:
        print(ll)
        print("in with")
    


    print(nodes[1].val)
