#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       python23.py
Create Time:    2015-07-14 20:12
Description:
    A class that represents thread-local data. Thread-local data are data whose values are thread specific.
"""
import threading

__all__ = ["threadlocal"]


class threadlocal(object):

    """
    A class that represents thread-local data
    """

    def __getattribute__(self, name):
        print "__getattribute__ called"
        if name == "__dict__":
            return threadlocal._getd(self)
        else:
            try:
                return object.__getattribute__(self, name)
            except AttributeError:
                try:
                    return self.__dict__[name]
                except KeyError:
                    raise AttributeError, name

    def __setattr__(self, k, v):
        print "__setattr__ called"
        self.__dict__[k] = v

    def __delattr__(self, k):
        try:
            del self.__dict__[k]
        except KeyError:
            raise AttributeError, k

    def _getd(self):
        print "_getd called"
        t = threading.currentThread()
        if not hasattr(t, '_d'):
            t._d = {}

        _id = id(self)
        if _id not in t._d:
            t._d[_id] = {}
        print "t._d={}".format(t._d)
        return t._d[_id]


if __name__ == '__main__':
    d = threadlocal()
    d.x = {'xuechao': 'zhao'}
    print "#####################"
    print "d.__dict__={}".format(d.__dict__)
    print "#####################"
    print d.x
