#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       doargs.py
Create Time:    2015-10-23 01:08
Description:
    process args                
"""
import sys

def do_arg(ch, value):
    print("{} ch={}, value={}".format(
        sys._getframe().f_code.co_name, ch, value))


def illegal_args(ch):
    print("{} ch={}".format(
        sys._getframe().f_code.co_name, ch))


def do_args(argc, argv, control, do_args, illegal_args):
    
    idx = 0 
    while idx < argc: 
        if argv[idx][0] == '-':
            if  argv[idx][1] in control:
                # do process
                if control[argv[idx][1]] == "": # no argument
                    do_arg(argv[idx][1], "")
                    idx += 1
                else: # with argument
                    if len(argv[idx]) > 2:
                        do_arg(argv[idx][1], argv[idx][2:])
                        idx += 1 
                    elif len(argv[idx]) == 2:
                        if idx + 1 < argc: 
                            do_arg(argv[idx][1], argv[idx+1])
                            idx += 2
                        else:
                            illegal_args(argv[idx][1])
                            idx += 1
            else:
                illegal_args(argv[idx][1])
                idx += 1
        else:
            illegal_args(argv[idx])
            idx += 1

if __name__ == "__main__":
    argv = ("-x", "-y", "z")
    argc = len(argv)

    control = {"x":""}
    do_args(argc, argv, control, do_args, illegal_args)
