#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       httpserver.py
Create Time:    2015-07-15 01:47
Description:

"""
server = None


def runsimple(func, server_address=("0.0.0.0", 8080)):
    # later func has to take static and log middleware.

    server = WSGIServer(server_address, func)

    try:
        server.start()
    except (KeyboardInterrupt, SysExit):
        server.stop()
        server = None


def WSGIServer(server_address, wsgi_app):
    import wsgiserver

    server = wsgiserver.CherryPyWSGIServer(
        server_address, wsgi_app, server_name="localhost")
    return server


if __name__ == '__main__':
    # start here....
    pass
