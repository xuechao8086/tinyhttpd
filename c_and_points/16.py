#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       16.py
Create Time:    2015-10-23 05:44
Description:
    exercise 16                
"""
import random
import time


def get_base(num):
    for radix in xrange(2, 36):
        #note : c has div function. 
        main = num // radix
        left = num % radix
        
        if main <= 2 and left <=9:
            return (radix, main*10+left)
      

def dicenum(maxnum):
    random.seed(time.time())    
    r = [0 for i in xrange(0, 6)]
    begin = time.time()
    for j in xrange(maxnum): 
        r[random.randint(1, 6)-1] += 1
    end = time.time()


    for i in xrange(0, 5):
        print("{}, {}".format(i+1, r[i]))
     
    print("{} cost time {}s".format(maxnum, end-begin))


if __name__ == '__main__':

    #print(get_base(29))
    
    #print(get_base(42))
    #print(get_base(41))
    for i in (99999,
              999999,
              9999999,
              99999999,
              999999999):
        dicenum(i)
