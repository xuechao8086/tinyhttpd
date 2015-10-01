#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       mxcldarr.py
Create Time:    2015-09-27 14:34
Description:
                
"""
STOCKPRICE = [ 100, 113, 110, 85, 105, 102, 86, 63, 81, 101, 94, 106, \
               101, 79, 94, 90, 97 ]

def find_max_sub_arr_one():
    """voilence way"""
    max = -float("inf")
    buyin, buyout = 0, 0
    for i in xrange(0, len(STOCKPRICE)):
        for j in xrange(i+1, len(STOCKPRICE)):
            dif = STOCKPRICE[j] - STOCKPRICE[i]
            if dif > max:
                max = dif
                buyin, buyout = i, j
    
    return buyin, buyout, max


def find_max_crossing_subarray(arr, low, mid, high):
    left_sum = -float("inf")
    sum, max_left = 0, mid
    for i in xrange(mid, low-1, -1):
        sum += arr[i]
        if sum > left_sum:
            left_sum = sum
            max_left = i
    
    right_sum = -float("inf")
    sum, max_right = 0, mid+1
    for j in xrange(mid+1, high+1, 1):
        sum += arr[j]
        if sum > right_sum:
            right_sum = sum
            max_right = j
    
    return max_left, max_right, left_sum+right_sum
      

def find_maximum_subarray(arr, low, high):
    if high == low:
        return (low, high, arr[low])
    else:
        mid = (low+high)/2
        left_low, left_high, left_sum = find_maximum_subarray(arr, low, mid)
        right_low, right_high, right_sum = find_maximum_subarray(arr, mid+1, high)
        cross_low, cross_high, cross_sum = find_max_crossing_subarray(arr, low, mid, high)
    
    if left_sum >= right_sum and left_sum >= cross_sum:
        return left_low, left_high, left_sum
    elif right_sum >= left_sum and right_sum >= cross_sum:
        return right_low, right_high, right_sum
    else:
        return cross_low, cross_high, cross_sum


def find_max_sub_arr_two():
    """recurrence way"""
    stock_price_change = [ STOCKPRICE[i] - STOCKPRICE[i-1]\
            for i in xrange(1, len(STOCKPRICE)) ]
    stock_price_change.insert(0, 0)
    
    low = 1
    high = len(STOCKPRICE) - 1
    
    _low, _high, _sum = find_maximum_subarray(stock_price_change, 
            1, high) 
    
    return _low-1, _high, _sum

if __name__ == '__main__':
    print(find_max_sub_arr_one())
    print(find_max_sub_arr_two())
