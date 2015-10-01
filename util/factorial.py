#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       factorial.py
Create Time:    2015-09-25 18:35
Description:
    factorial.py
"""

def new(cls, *args, **kwargs):
    return cls(*args, **kwargs)


class Number(object):
    pass


class IntegralNumber(int, Number):
    def toInt(self):
        return new (int, self)


class InternalBase(object):
    def __init__(self, base):
        self.base = base.toInt()
    def getBase(self):
        return new (IntegralNumber, self.base)


class MathematicsSystem(object):
    def __init__(self, ibase):
        pass

    @classmethod
    def getInstance(cls, ibase):
        try:
            cls.__instance
        except AttributeError:
            cls.__instance = new (cls, ibase)
        return cls.__instance


class StandardMathematicsSystem(MathematicsSystem):
    def __init__(self, ibase):
        if ibase.getBase() != new (IntegralNumber, 2):
            raise NotImplementedError
        self.base = ibase.getBase()

    def calculateFactorial(self, target):
        result = new (IntegralNumber, 1)
        i = new (IntegralNumber, 2)
        while i <= target:
            result = result * i
            i = i + new (IntegralNumber, 1)
        return result

print StandardMathematicsSystem.getInstance(new (InternalBase, new (IntegralNumber, 2))).calculateFactorial(new (IntegralNumber, 6))

fact = lambda x: reduce(int.__mul__, xrange(2, x + 1), 1)
print fact(6)

if __name__ == '__main__':
    pass
