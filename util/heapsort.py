#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       heapsort.py
Create Time:    2015-09-09 18:47
Description:
                
"""
import random


class HeapType(object):
    def __init__(self, length, a=0, b=100):
        self.arr = [random.randint(a, b) 
                    for i in xrange(length) ]
     
        self.length = length
        

def heapadjust(ht, s, m):
    rc = ht.arr[s]
    
    j = 2*s
    while j <= m:
        if j < m and ht.arr[j] < ht.arr[j+1]:
            j = j+1
        if not rc < ht.arr[j]:
            break
        ht.arr[s] = ht.arr[j]
        s = j

        j *= 2

    ht.arr[s] = rc


def heapsort(ht):
    for i in xrange(ht.length/2-1, 0, -1):
        heapadjust(ht, i, ht.length-1)

    for i in xrange(ht.length-1, 0, -1):
        ht.arr[0], ht.arr[i] = ht.arr[i], ht.arr[1] 
        heapadjust(ht, 0, i-1)


if __name__ == '__main__':
    for i in xrange(10): 
        ht = HeapType(random.randint(10, 100)) 
        heapsort(ht)
        print ht.arr
        print ''
