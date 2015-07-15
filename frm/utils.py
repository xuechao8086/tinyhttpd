#!/usr/bin/env python2.7
# coding:utf-8

"""
Author:         charliezhao
Filename:       utils.py
Create Time:    2015-07-14 20:05
Description:

"""
from fileinput import filename


__all__ = [
    "Storage", "storage", "storify",
    "Counter", "counter"

]

import re
import sys
import time
import threading
import itertools
import traceback
import os

try:
    import subprocess
except ImportError:
    subprocess = None

try:
    import datetime
except ImportError:
    pass

try:
    import set
except NameError:
    from sets import Set as set
except ImportError:
    from sets import Set as set

try:
    from threading import local as threadlocal
except ImportError:
    from python23 import threadlocal


class Storage(dict):

    """
    a storage object is like a dictionary except `obj.foo` can be used
    in addition to `obj[`foo`]
    """

    def __getattr__(self, key):
        try:
            return self[key]
        except KeyError, k:
            raise AttributeError, k

    def __setattr__(self, key, value):
        self[key] = value

    def __delattr__(self, key):
        try:
            del self[key]
        except KeyError, k:
            raise AttributeError, k

    def __repr__(self):
        return "<Storage " + dict.__repr__(self) + ">"

storage = Storage


def safeunicode(obj, encoding='utf-8'):
    """
    convert any giving object to unicode string.
    """
    t = type(obj)
    if t is unicode:
        return obj
    elif t is str:
        return obj.decode(encoding)
    elif t in (int, float, bool):
        return unicode(obj)
    elif hasattr(obj, '__unicode__') or isinstance(obj, unicode):
        return unicode(obj)
    else:
        return str(obj).decode(encoding)


def storify(mapping, *requireds, **defaults):
    _unicode = defaults.pop("_unicode", False)

    to_unicode = safeunicode
    if _unicode is not False and hasattr(_unicode, "__call__"):
        to_unicode = _unicode

    def unicodify(s):
        if _unicode and isinstance(s, str):
            return to_unicode(s)
        else:
            return s

    def getvalue(x):
        if hasattr(x, 'file') and hasattr(x, 'value'):
            return x.value
        elif hasattr(x, 'value'):
            return unicodify(x.value)
        else:
            return unicodify(x)

    stor = Storage()
    for key in requireds + tuple(mapping.keys()):
        value = mapping[key]
        if isinstance(value, list):
            if isinstance(defaults.get(key), list):
                value = [getvalue(x) for x in value]
            else:
                value = value[-1]
        if not isinstance(defaults.get(key), dict):
            value = getvalue(value)
        if isinstance(defaults.get(key), list) and not isinstance(value, list):
            value = [value]
        setattr(stor, key, value)

    for (key, value) in defaults.iteritems():
        result = value
        if hasattr(stor, key):
            result = stor[key]
        if value == () and not isinstance(result, tuple):
            result = (result,)
        setattr(stor, key, result)

    return stor


class Counter(Storage):

    """
    keeps count of how many times something is added
    """

    def add(self, n):
        self.setdefault(n, 0)
        self[n] += 1

    def most(self):
        m = max(self.itervalues())
        return [k for k, v in self.iteritems() if v == m]

    def least(self):
        m = min(self.itervalues())
        return [k for k, v in self.iteritems() if v == m]

    def percent(self, key):
        return float(self[key]) / sum(self.values())

    def sorted_keys(self):
        return sorted(self.keys(), key=lambda k: self[k], reverse=True)

    def sorted_values(self):
        return [self[k] for k in self.sorted_keys()]

    def sorted_items(self):
        return [(k, self[k]) for k in self.sorted_keys()]

    def __repr__(self):
        return '<Counter ' + dict.__repr__(self) + '>'

counter = Counter


def _charlie_test():
    _x = storage(value='a')
    print "_x.__dict__={}".format(_x.__dict__)
    print _x.value, _x['value']
    _y = storify({'x': _x}, _unicode=True)
    print "_y.__dict__={}".format(_y.__dict__)
    a = Storage({"xuechao": "zhao", "tingting": "zhang"})
    print a["xuechao"]
    print a.__dict__
    print a.xuechao

    c = Counter()
    c.add('x')
    c.add('x')
    c.add('y')

    print c.sorted_keys()
    print c.sorted_values()
    print c.sorted_items()


if __name__ == '__main__':
    # start here....
    _charlie_test()
