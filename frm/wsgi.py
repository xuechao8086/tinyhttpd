#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       wsgi.py
Create Time:    2015-07-15 01:44
Description:
                
"""
import os, sys

import http
import webapi as web
from utils import listget, intget
from net import validaddr, validip
import httpserver
 

def runfcgi(func, addr=('localhost', 8000)):
    """Runs a WSGI function as a FastCGI server."""
    pass

def runscgi(func, addr=('localhost', 4000)):
    """Runs a WSGI function as an SCGI server."""
    import flup.server.scgi as flups
    pass

def runwsgi(func):
    """
    Runs a WSGI-compatible `func` using FCGI, SCGI, or a simple web server,
    as appropriate based on context and `sys.argv`.
    """
    print func
    server_addr = validip(listget(sys.argv, 1, ''))
    if os.environ.has_key('PORT'):
        server_addr = ('0.0.0.0', intget(os.environ['PORT']))
    return httpserver.runsimple(func, server_addr)


if __name__ == '__main__':
    pass 
