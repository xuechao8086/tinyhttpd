#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       test2.py
Create Time:    2015-07-29 06:44
Description:
    it use binary value by libmemcached 
"""

import pylibmc
mc = pylibmc.Client(["127.0.0.1"], binary=True,
        behaviors={"tcp_nodelay": True,
            "ketama": True})
mc["some_key"] = "Some value"
print mc.get("some_key")

if __name__ == '__main__':
    pass
