#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       inormal.py
Create Time:    2015-09-19 21:02
Description:
                
"""
import sys
import time
import random
import weakref

class CallbackProperty(object):
    """A property that will alert observers when upon updates"""
    def __init__(self, default=None):
        self.data = weakref.WeakKeyDictionary()
        self.default = default
        self.callbacks = weakref.WeakKeyDictionary()
 
    def __get__(self, instance, owner):
        if instance is None:
            return self        
        return self.data.get(instance, self.default)
 
    def __set__(self, instance, value):
        for callback in self.callbacks.get(instance, []):
            # alert callback function of new value
            callback(value)
        self.data[instance] = value
 
    def add_callback(self, instance, callback):
        """Add a new function to call everytime the descriptor within instance updates"""
        if instance not in self.callbacks:
            self.callbacks[instance] = []
        self.callbacks[instance].append(callback)
 
class BankAccount(object):
    balance = CallbackProperty(0)
 
def low_balance_warning(value):
    if value < 100:
        print "You are now poor"
 
ba = BankAccount()
BankAccount.balance.add_callback(ba, low_balance_warning)
 
ba.balance = 5000
print "Balance is %s" % ba.balance
ba.balance = 99
print "Balance is %s" % ba.balance

sys.exit()



#for abstract 
import abc
class MyABC(object):
    __metaclass__ = abc.ABCMeta

MyABC.register(tuple)
print(issubclass(tuple, MyABC))
print(isinstance((), MyABC))
a = (1, 2, 3)


#for with
class echo :
    def output(self) :
        print 'hello world'
        
    def __enter__(self): 
        print 'enter'
        return self #返回自身实例，当然也可以返回任何希望返回的东西
    
    def __exit__(self, exception_type, exception_value, exception_traceback): #若发生异常，会在这里捕捉到，可以进行异常处理
        print 'exit'
        #如果改__exit__可以处理改异常则通过返回True告知该异常不必传播，否则返回False 
        if exception_type == ValueError :
            return True
        else:
            return False
with echo() as e:
    e.output()


#for with one
class Echo:
    def output(self):
        print("output")

    def __enter__(self):
        print("__enter__ called")
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")

class Echo2(object):
    def __init__(self):
        pass

    def output(self):
        print("output")

    def __enter__(self):
        print("__enter__ called")
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        print("__exit__ called")

with Echo2() as e:
    e.output()

#for with
from contextlib import contextmanager

@contextmanager
def make_context() :
    print 'enter'
    try :
        yield {}
    except RuntimeError, err :
        print 'error' , err
    finally :
        print 'exit' 
        
with make_context() as value : 
    print value 


#for list
def fast():
    a = [ i for i in xrange(100000) ]
    sum = 0
    t1 = time.time()
    for i in a:
        sum += i
    t2 = time.time()
    print("fast time cost =", t2 - t1)

def slow():
    #a = [i for i in xrange(10000000) ]
    #random.shuffle(a)
    a = [random.randint(0, 100000) for i in xrange(100000)]
    sum = 0
    t1 = time.time()
    for i in a:
        sum += i
    t2 = time.time()
    print("slow time cost =", t2 - t1);

fast()
slow()

#for static, abstract, class method
class PPizza(object):
    def get_radius(self):#abstract
        raise NotImplementedError

class Pizza(PPizza):
    def get_radius(self): #abstract in father
        return 10

    @staticmethod
    def mix_ingredients(x, y):
        return x + y 
    
    @classmethod
    def mix_ingredients2(cls, x, y):
        return x + y
    

    def cook(self): 
        return self.mix_ingredients(self.cheese, self.vegetables)

p = Pizza()
print p.get_radius()
print Pizza.mix_ingredients(3, 5)
print Pizza.mix_ingredients2(3, 5)
print p.mix_ingredients(3, 5)
print p.mix_ingredients2(3, 5)

#better abstract way
import abc 
class BasePizza(object):
    __metaclass__ = abc.ABCMeta
    
    def get_foo(self):
        return 'foo'

    @abc.abstractmethod
    def get_radius(self):
        pass

#b = BasePizza()
#b.get_foo()
#b.get_radius()

#for self key words
class Parent(object):
    def pptr(self):
        print("self=", self)
    
class Child(Parent):
    def cptr(self):
        print("self=", self)

c = Child()
c.cptr()
c.pptr()

class Desc(object):
    def __get__(self, ins, cls):
        print 'self in Desc: %s ' % self 
        print self, ins, cls

class Test(object):
    x = Desc()
    def prt(self):
        print 'self in Test: %s' % self

t = Test()
t.prt()
t.x


#for args
def foo(bar=[]):
    bar.append("xuechao")
    return bar

for i in xrange(10):
    print foo()


def foo(a):
    x = 3
    return x + a

#for scope, local scope faster than global scope
import dis
dis.dis(foo.func_code)

def not_list_or_dict(value): return not (isinstance(value, dict) or isinstance(value, list)) 
not_list_or_dict(50)



def not_list_or_dict(value, _isinstance=isinstance, _dict=dict, _list=list): return not (_isinstance(value, _dict) or _isinstance(value, _list))
not_list_or_dict(50)


#for variable bug.
def foo(lst):
    a = 0
    for i in lst:
        a += i
    
    b = 1
    for t in lst:
        b *= i
    
    return a, b

print "foo([i for i in xrange(1, 4) ])=",  foo([i for i in xrange(1, 4) ])

#for dynamic
def modules(x, y):
    return x % y

print "module(7, 4) = ", modules(7, 4)
print 'modules("xuechao %s", "zhao") = ', modules("xuechao %s", "zhao")


class Suprise(object):
    def __init__(self, num):
        self.num = num

    def __mod__(self, other):
        return self.num + other.num

a, b = Suprise(7), Suprise(4)
print "modules(Suprise(7), Suprise(4) = ", modules(a, b)

#for class settter and getter
class Celsius(object):
    def __init__(self, temperature=0):
        self.set_temperature(temperature)

    def to_fahrenheit(self):
        return (self.get_temperature() * 1.8 ) + 32
    
    def get_temperature(self):
        print "call get_temperature"
        return self._temperature

    def set_temperature(self, value):
        print "call set_temperature"
        self._temperature = value#outside visiable
        self.__temperature = value#outside not visiable
    
    temperature = property(get_temperature, set_temperature)

c = Celsius(-300)


print "c._temperature = ", c._temperature
print "c.temperature = ", c.temperature

#for class settter and getter, better way!
class Celsius(object):
    def __init__(self, temperature=0):
        self._temperature = temperature 

    def to_fahrenheit(self):
        return (self.get_temperature() * 1.8 ) + 32
    
    @property 
    def temperature(self):
        return self._temperature
    
    @temperature.getter
    def get_temperature(self):
        print "call get_temperature"
        return self._temperature
    
    @temperature.setter
    def set_temperature(self, value):
        print "call set_temperature"
        self._temperature = value#outside visiable
        self.__temperature = value#outside not visiable
    
c = Celsius(-300)


print "c.temperature = ", c.temperature

#for decorader
import time
def timeit(func):
    def wrapper(*args, **kwargs):
        start = time.time()
        func(*args, **kwargs)
        end = time.time()
        print func, "cost time =", end-start, "s"
    return wrapper

@timeit
def foo(secs, name="xuechao"):
    time.sleep(secs) 

foo(1)

#for property, better way!

from weakref import WeakKeyDictionary
 
class NonNegative(object):
    """A descriptor that forbids negative values"""
    def __init__(self, default):
        self.default = default
        self.data = WeakKeyDictionary()
 
    def __get__(self, instance, owner):
        # we get here when someone calls x.d, and d is a NonNegative instance
        # instance = x
        # owner = type(x)
        return self.data.get(instance, self.default)
 
    def __set__(self, instance, value):
        # we get here when someone calls x.d = val, and d is a NonNegative instance
        # instance = x
        # value = val
        if value < 0:
            raise ValueError("Negative value not allowed: %s" % value)
        self.data[instance] = value
 
class Movie(object):
 
    #always put descriptors at the class-level
    rating = NonNegative(0)
    runtime = NonNegative(0)
    budget = NonNegative(0)
    gross = NonNegative(0)
 
    def __init__(self, title, rating, runtime, budget, gross):
        self.title = title
        self.rating = rating
        self.runtime = runtime
        self.budget = budget
        self.gross = gross
 
    def profit(self):
        return self.gross - self.budget
 
m = Movie('Casablanca', 97, 102, 964000, 1300000)
print m.budget  # calls Movie.budget.__get__(m, Movie)
m.rating = 100  # calls Movie.budget.__set__(m, 100)
try:
    m.rating = -1   # calls Movie.budget.__set__(m, -100)
except ValueError:
    print "Woops, negative value"


#for property, carefully!
class BrokenNonNegative(object):
    def __init__(self, default):
        self.value = default 
    
    def __get__(self, instance, owner):
        return self.value 
    
    def __set__(self, instance, value):
        if value < 0:
            raise ValueError("Negative value not allowed: %s" % value)
        self.value = value

class Foo(object):
    bar = BrokenNonNegative(5) 

f = Foo()
g = Foo()
 
print "f.bar is %s\ng.bar is %s" % (f.bar, g.bar)
print "Setting f.bar to 10"
f.bar = 10
print "f.bar is %s\ng.bar is %s" % (f.bar, g.bar)  #ouch


#another way! for property

class Descriptor(object):
    def __init__(self, label):
         self.label = label 
         
    def __get__(self, instance, owner):
        print '__get__', instance, owner
        return instance.__dict__.get(self.label) 
    
    def __set__(self, instance, value):
        print '__set__' 
        instance.__dict__[self.label] = value 
        

class Foo(object): 
    x = Descriptor('x') 
    y = Descriptor('y')
    
    def __init__(self, x):
        self.x = x

f = Foo(10)
f.x = 5 
print f.x 

#for meta class!
class Descriptor(object):
    def __init__(self):
        #notice we aren't setting the label here
        self.label = None 
    
    def __get__(self, instance, owner):
        print '__get__. Label = %s' % self.label
        return instance.__dict__.get(self.label, None) 
    
    def __set__(self, instance, value):
        print '__set__'
        instance.__dict__[self.label] = value

class DescriptorOwner(type):
    def __new__(cls, name, bases, attrs):
        # find all descriptors, auto-set their labels
        for n, v in attrs.items():
            if isinstance(v, Descriptor):
                v.label = n
        return super(DescriptorOwner, cls).__new__(cls, name, bases, attrs) 

#for self key words!
class Foo(object):
    __metaclass__ = DescriptorOwner
    x = Descriptor() 

f = Foo()
f.x = 10
print f.x 

class Test(object): 
    @classmethod
    def prt(cls): 
        print(cls)

Test.prt()

class Test: 
    @classmethod
    def prt(cls): 
        print cls

Test.prt()

print 'aerdfas'
if __name__ == '__main__':
    pass
