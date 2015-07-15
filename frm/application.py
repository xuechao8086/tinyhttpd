#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       application.py
Create Time:    2015-07-15 02:04
Description:
                
"""
import wsgi

__all__ = [
        "application",
        ]

class application:
    def __init__(self, mapping=(), fvars={}, autoload=None):
        self.init_mapping(mapping)
        self.fvars = fvars
        self.processors = []

        self.add_processor(loadhook(self._load))
        self.add_processor(unloadhook(self._unload))

    def init_mapping(self, mapping):
        pass

    def _load(self):
        pass

    def _unload(self):
        pass

    def add_processor(self, processor):
        self.processors.append(processor)

    def run(self, *middleware):
        return wsgi.runwsgi(self.wsgifunc(*middleware))
    
    def wsgifunc(self, *middleware):
        pass

def loadhook(h):
    def processor(handler):
        h()
        return handler()

    return processor

def unloadhook(h):
    pass

if __name__ == '__main__':
    pass
