#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       primnum.py
Create Time:    2015-10-21 00:55
Description:
                
"""
def get_prime_num(limit):
    array = [ True for i in xrange(0, limit) ]
    
    for i in xrange(2, limit):
        for j in xrange(i+1, limit):
            if j % i  == 0:
                array[j] = False
    
    return [ i for i in xrange(1, limit) if array[i]]



#consider how to use map



if __name__ == '__main__':
    print(get_prime_num(1024))
