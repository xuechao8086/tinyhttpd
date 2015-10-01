#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       dynamicprogramming.py
Create Time:    2015-09-28 19:52
Description:
    dynamic programming                
"""


def cut_rod(p, n, points={}):
    """consider how to record points, here it has bug!"""
    if n == 0:
        return 0, points
    q = -float("inf")
    
    for i in xrange(1, n+1):
        t = p[i] + cut_rod(p, n-i, points)[0]
        if t > q:
            if n not in points: 
                points.append(i)
    return q, points


def cut_rod(p, n):
    if n == 0:
        return 0
    q = -float("inf")
    for i in xrange(1, n+1):
        q = max(q, p[i] + cut_rod(p, n-i))
    return q


def memoized_cut_rod(p, n):
    r = [-float("inf") for i in xrange(0, n+1)]
    return memoized_cut_rod_aux(p, n, r)


def memoized_cut_rod_aux(p, n, r):
    if r[n] >= 0:
        # print("point=",n)
        return r[n]
    if n == 0:
        q = 0
    else:
        q = -float("inf")
        for i in xrange(1, n+1):
            q = max(q, p[i]+memoized_cut_rod_aux(p, n-i, r))
    r[n] = q
    # print("point=",n)
    return q


def bottom_up_cut_rod(p, n):
    r = [0 for i in xrange(0, n+1)]
    for j in xrange(1, n+1):
        q = -float("inf")
        for i in xrange(1, j+1):
            q = max(q, p[i]+r[j-i])
        r[j] = q
    return r[n]


if __name__ == '__main__':
    price = [0, 1, 5, 8, 9, 10, 17, 17, 20, 24, 30]
    print(cut_rod(price, 4))
    print(memoized_cut_rod(price, 4))
    print(bottom_up_cut_rod(price, 4))
