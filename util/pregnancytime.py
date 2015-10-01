#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       pregnancytime.py
Create Time:    2015-10-01 15:38
Description:
    pregnancy time                
"""
import os
import time

def getpregancy_time(begin, end):
    """
    `param begin`: str time
    `param end`: str time
    """
    begin_time_tuple = time.strptime(begin, "%Y-%m-%d %H:%M:%S")
    end_time_tuple = time.strptime(end, "%Y-%m-%d %H:%M:%S")
    
    beginstamp = time.mktime(begin_time_tuple)
    endstamp = time.mktime(end_time_tuple)
    
    days = (endstamp-beginstamp)//(24*3600)
    weeks = days//7
    days2 = days%7

    print("pregnancy time: total {} days({} weeks and {} days)".\
            format(days, weeks, days2))


def timezone(zone):
    """set time zone"""
    os.environ['TZ'] = zone
    time.tzset()


if __name__ == "__main__":
    zone="GMT-8"
    timezone(zone) 

    now = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    fertilizationend_time = "2015-09-03 00:00:00"
    menstrual_time = "2015-08-19 00:00:00"
    
    print("from {} to {} {}:".format(menstrual_time, now, zone))
    getpregancy_time(menstrual_time, now)
    print("from {} to {} {}:".format(fertilizationend_time, now, zone))
    getpregancy_time(fertilizationend_time, now)
