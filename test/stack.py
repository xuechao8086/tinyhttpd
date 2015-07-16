#!/usr/bin/env python3.4
#coding:utf8
"""
Author:         charliezhao
Filename:       stack.py
Create Time:    2015-07-16 02:17
Description:

"""
import sys
def func(i):
    print( "stack depth={}".format(i))
    try:
        func(i+1)
    except RuntimeError:
        print( 'RuntimeError')
        sys.exit()
    if i == 100000:
        return True



if __name__ == '__main__':
    #start here....
    func(1)
