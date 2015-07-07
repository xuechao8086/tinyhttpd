#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       __init__.py
Create Time:    2015-07-05 05:02
Description:
    reactor for platforms.                
"""
import select 
import sys

use_mod = None

if "epoll" in select.__dict__:
    use_mod = "epoll"
    from epollreactor import EpollReactor as Reactor
    #from selectreactor import SelectReactor as Reactor
elif "kqueue" in select.__dict__:
    use_mod = "kqueue"
    from kqueuereactor import KqueueReactor as Reactor
elif "poll" in select.__dict__:
    use_mod = "poll"
    from pollreactor import PollReactor as Reactor
elif "select" in select.__dict__:
    use_mod = "select"
    from selectreactor import SelectReactor as Reactor
else:
    print "there is no reactor can be used"
    assert False


if __name__ == "__main__":
    pass 
