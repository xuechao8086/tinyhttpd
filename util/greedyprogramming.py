#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       greedyprogramming.py
Create Time:    2015-09-29 16:55
Description:
    greedy algorithm                
"""


def recursive_activity_selector(s, f, k, n, r):
    """has bug, need fix"""
    m = k+1
    while m < n and s[m] < f[k]:
        m += 1
    if m < n:
        r.append(m)
        return recursive_activity_selector(s, f, m, n, r)
    else:
        return None
        
if __name__ == '__main__':
    activities = {1:(1, 4),
                  2:(3, 5),
                  3:(0, 6),
                  4:(5, 7),
                  5:(3, 9),
                  6:(5, 9),
                  7:(6, 10),
                  8:(8, 11),
                  9:(8, 12),
                  10:(2, 14),
                  11:(12, 16)
                  }
    arr = sorted(activities.iteritems(), key=lambda k:k[1][1])
    s = map(lambda k:k[1][0], arr)
    f = map(lambda k:k[1][1], arr)
    k = 0
    n = len(arr)
    r = [] 
    print(recursive_activity_selector(s, f, k, n, r))
    print r
