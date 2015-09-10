#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       bitree.py
Create Time:    2015-09-10 15:08
Description:
                
"""
import sys


class TreeNode(object):
    def __init__(self, data="", left=None, right=None):
        self.data = data
        self.left = left
        self.right = right


class BTree(object):
    def __init__(self, root=0):
        self.root=root


def create_bi_tree(t):
    data = raw_input("$")
    data = data.strip()
    if data != "":
        t.data=data
        t.left = TreeNode()
        create_bi_tree(t.left)
        t.right = TreeNode()
        create_bi_tree(t.right)


def pre_order_traverse(prefix, t):
    if t == None:
        return
    if t.data != "":
        print "{}{}".format(prefix, t.data)
    pre_order_traverse("{}{}".format(prefix, '<'),
                       t.left)
    pre_order_traverse("{}{}".format(prefix, '>'),
                       t.right)


def create_bi_tree_test(t):
    t.data = "name"
    t.left = TreeNode("xuechao", 0, 0)
    t.right = TreeNode("tingting", 0, 0)


def print_bi_tree_test(t):
    print t.data
    print "<{}".format(t.left.data)
    print ">{}".format(t.right.data)


def recurse_print(i):
    if i == 0:
        return i
    else:
        return "{}{}".format(i, recurse_print(i-1))


def recurse_print2(prefix, i):
    if i == 0:
        print "{}{}".format(prefix, i)
    else:
        return recurse_print2("{}{}".format(prefix, i),
                              i-1)


if __name__ == '__main__':
    #start here....
    print "some test"
    print recurse_print(10) 
    recurse_print2('',10)
    print "ok, can go!"

    t = TreeNode()
    create_bi_tree(t)

    pre_order_traverse('', t)


