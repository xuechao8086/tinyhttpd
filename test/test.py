#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       test.py
Last modified:  2015-04-30 19:14
Description:
                
"""
import sys
import time
def iterview(x):
   """
   Takes an iterable `x` and returns an iterator over it
   which prints its progress to stderr as it iterates through.
   """
   WIDTH = 70

   def plainformat(n, lenx):
       return '%5.1f%% (%*d/%d)' % ((float(n)/lenx)*100, len(str(lenx)), n, lenx)

   def bars(size, n, lenx):
       val = int((float(n)*size)/lenx + 0.5)
       if size - val:
           spacing = ">" + (" "*(size-val))[1:]
       else:
           spacing = ""
       return "[%s%s]" % ("="*val, spacing)

   def eta(elapsed, n, lenx):
       if n == 0:
           return '--:--:--'
       if n == lenx:
           secs = int(elapsed)
       else:
           secs = int((elapsed/n) * (lenx-n))
       mins, secs = divmod(secs, 60)
       hrs, mins = divmod(mins, 60)

       return '%02d:%02d:%02d ' % (hrs, mins, secs)

   def format(starttime, n, lenx):
       out = plainformat(n, lenx) + ' '
       if n == lenx:
           end = '     '
       else:
           end = ' ETA '
       end += eta(time.time() - starttime, n, lenx)
       out += bars(WIDTH - len(out) - len(end), n, lenx)
       out += end
       return out

   starttime = time.time()
   lenx = len(x)
   for n, y in enumerate(x):
       sys.stderr.write('\r' + format(starttime, n, lenx))
       yield y
   sys.stderr.write('\r' + format(starttime, n+1, lenx) + '\n')

for i in iterview([i for i in xrange(10)]):
    print i


a = {'xuechao':'zhao',
     'tingting':'zhang',
     'charlie':'zhao'
     }
for i in iterview(a):
    print i



sys.exit()
class TestCls(object):
    
    def __call__(self):
        print '__call__ called'

a = TestCls
print TestCls()
print a()()
print 'here'
sys.exit()


def test():
    print "test func"

print dir(test)
print dir(test.__code__)
print str(test.__code__.co_code)
sys.exit()


import os
import sys
import time


for i in xrange(0, 10):
    pid = os.fork()
    if pid == 0:
        break

print "pid = {0}".format(pid)

sys.exit()



for i in xrange(0, 10):
    pid = os.fork()
    if pid:
        print "father process, pid:{0}".format(os.getpid())
    else:
        print "child pid: pid:{0}".format(os.getpid())

    time.sleep(1)
sys.exit()



import os, sys
import subprocess
import zlib
import StringIO
import gzip
import getpass


print getpass.getuser()
print getpass.getpass()





sys.exit()

buf = StringIO.StringIO()
f = gzip.GzipFile(mode='wb', fileobj=buf)

with open('./dat', 'r') as f :
    contents = f.read()

f = gzip.GzipFile(mode='wb', fileobj=buf)
f.write(contents)
f.close()

res = contents
res = buf.getvalue()
print res

sys.exit()





with open('./dat', 'r') as f:
    for line in f:
        if line.find(' = ') == -1:
            print ''
            continue
        k, v = line.split(' = ')
        v = '{0} {1}'.format(k.replace('_', ''), v.replace("'", '').replace("\n", ""))
        print "{0} = '{1}'".format(k, v)




sys.exit()
class TestCls(object):
    _1 = 'xuechao'
    def __init__(self):
        self._2 = 'zhao'
class TestClsC(TestCls):

    def __init__(self):
        super(TestCls, self).__init__()
inst = TestClsC()
print inst._1
print inst._2
sys.exit()
class ClsA(object):
    def func1(self):

        b = {'xuechao':'zhao'} 
        func2()
        def func2():
            del b['xuechao']
        print 'b = {0}'.format(b)

a = ClsA()
a.func1()

def test2(a):
    a['name'] = 'charlie'

a={'first':{'name':'xuechao'}, 'second':{'name':'tingting'}}
print 'global a={0}'.format(a)
test2(a['first'])
print 'global a={0}'.format(a)



def test_func(name, *args, **kwargs):
    print name
    print args
    print kwargs

test_func('xuechoa', 1, 2, 3, 4, 5, fail='xuechao')




import sys
sys.exit()
def func2():
    print DATA_ID_COUNT



if __name__ == '__main__':
    DATA_ID_COUNT = 100
    func2()
    sys.exit()

def print_hello():
    print 'hello, charliezhao'

import  os
import sys
import time
import inspect

def _filename():
    return os.path.basename(inspect.currentframe().f_code.co_filename)

def _linenum():
    try:
        raise Exception()
    except:
        return sys.exc_info()[2].tb_frame.f_back.f_lineno
            
def _debug_log(args_str=''):
    time_str = '{0} {1:.20f}'.format(
                time.strftime('%Y-%m-%d %H:%M:%S', time.localtime()),
                time.time())
    pid = os.getpid()
    with open('/tmp/swift-debug.log', 'a+') as f:
        #f.write("\033[31m{args_str}\033[0m\n".format(args_str=args_str))
        f.write("{time_str} {pid} {args_str}\n".format(
                    time_str=time_str,
                    pid=pid,
                    args_str=args_str))
    return 0




class Test(object):

    def func1(self):
        print 'Test.func1'

    def func2(self, args):
        print 'Test.func2 args=', args

a = Test()
print '1', getattr(a, 'func1')
print '2', getattr(a, 'func1')()
print '==='

