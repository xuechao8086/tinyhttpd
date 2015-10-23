#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       matrix.py
Create Time:    2015-10-21 18:28
Description:
    matrix func.                
"""
import pprint

m1 = [[2, -6],
      [3, 5],
      [1, -1]]

m2 = [[4, -2, -4, -5],
      [-7, -3, 6, 7]]

m3 = [[0 for i in xrange(4)] for j in xrange(3)]

for m in m1, m2, m3:
    pprint.pprint(m)
    print("")

for i in xrange(3):
    for j in xrange(4):
        m3[i][j] = sum(m1[i][k]*m2[k][j] for k in xrange(2))


pprint.pprint(m3)
if __name__ == '__main__':
    pass
