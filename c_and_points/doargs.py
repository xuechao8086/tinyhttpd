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
    # print("{} ch={}, value={}".format(
    #    sys._getframe().f_code.co_name, ch, value))
    
    print("do_arg({}, {})".format(ch, value))

def illegal_args(ch):
    # print("{} ch={}".format(
    #   sys._getframe().f_code.co_name, ch))
    print("illegal_args({})".format(ch))

def do_args(argc, argv, control, do_args, illegal_args):
    print("control = {}".format(control))
    print("argv = {}".format(argv))

    idx = 0 
    while idx < argc: 
        if argv[idx][0] == '-':
            if  argv[idx][1] in control:
                # do process
                if control[argv[idx][1]] == "": # no argument
                    if len(argv[idx]) == "2":
                        do_arg(argv[idx][1], None)
                        idx += 1
                    else:
                        idxlen = len(argv[idx])
                        i = 2
                        while i < idxlen:
                            if argv[idx][2] in control:
                                if i != idxlen - 1:
                                    do_args[argv[idx][2] # to be continue



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
    print("########################################")


if __name__ == "__main__":
    argv = ("-x", "-y", "z")
    argc = len(argv)

    control = {"x":""}
    #do_args(argc, argv, control, do_arg, illegal_args)

    for argv, control in (\
            #(("-x", "-y", "-z"),{"x":"+", "y":"+", "z":"+"}),
            (("-abcd", "-ef", "ghi", "jkl"), {"a":"", "b":"+", "c":"", "d":"", "e":"", "f":"+", "g":""}),
            #(("-a", "b", "-c", "-d", "-e", "-f"), {"a":"", "b":"", "c":"", "d":"", "e":"", "f":""}),
            ):
        argc = len(argv)
        do_args(argc, argv, control, do_arg, illegal_args)





