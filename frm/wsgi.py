#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       wsgi.py
Create Time:    2015-07-15 01:44
Description:
                
"""
import os, sys

import httpserver


__all__ = [
        "runwsgi"
        ]

def runwsgi(func):
    server_addr = ('0.0.0.0', 8080)
    return httpserver.runsimple(func,  server_addr)

if __name__ == '__main__':
    pass 
