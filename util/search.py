#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       search.py
Create Time:    2015-09-15 16:36
Description:
        search algorithm
"""
from __future__ import print_function
import random
import sys
import logging

class ElemType(object):
    def __init__(self, key):
        self.key = key


class SSTable(object):
    def __init__(self, elem, length):
        self.elem = elem
        self.length = length


def gen_sstable():
    elem = [ElemType(random.randint(1, 2**3))\
             for i in xrange(random.randint(2**3, 2**4))]
    length = len(elem)

    return SSTable(elem, length)


def gen_sstable_seq():
    elem = [ElemType(random.randint(1, 2**3))\
             for i in xrange(random.randint(2**3, 2**4))]
    elem.sort()
    length = len(elem)

    return SSTable(elem, length)


def output_sstable(st):
    print("##############################################################")
    print("sstable output:")
    ls = [ i.key for i in st.elem ]
    print(ls)


def search_seq(st, key):
    st.elem[0].key = key
    
    i = st.length - 1
    while st.elem[i].key != key:
        i -= 1
    return i
    

def search_bin(st, key):
    low, high = 0, st.length - 1
    
    while low <= high:
        mid = (low+high)/2
        if st.elem[mid].key == key:
            return mid
        elif st.elem[mid].key < key:
            low = mid + 1
        else:
            high = mid - 1


class TreeNode(object):
    def __init__(self, data, left=None, right=None, parent=None):
        self.data = data
        if left != None:
            self.left = left
        if right != None:
            self.right = right
        self.parent = parent


class BTree(object):
    def __init__(self, root=None):
        if root != None:
            self.root=root


def output_btree(prefix, t):
    print("{} {}".format(prefix, t.data))
    if getattr(t, 'left', None) != None:
        output_btree(prefix+"<", t.left)
    if getattr(t, 'right', None) != None:
        output_btree(prefix+">", t.right)


def search_bst(t, key, debug=False):
    if key == t.data:
        return (True, t, t.parent)
    elif key < t.data:
        if hasattr(t, 'left'):
            return search_bst(t.left, key, debug)
        else:
            return (False, t, t.parent)
    elif key > t.data:
        if hasattr(t, 'right'):
            return search_bst(t.right, key, debug)
        else:
            return (False, t, t.parent)


def insert_bst(t, e):
    if getattr(t, 'root', None) == None:
        t.root = TreeNode(e, parent=None)
        return t

    flag, p, pp = search_bst(t.root, e)
    if flag == False:
        s = TreeNode(e,parent=p)
        if p.data < e:
            p.right = s
        else:
            p.left = s
         
    return t


def delete(p):
    """
    `param p`: TreeNode
    """
    def free(p):
        if p.data < p.parent.data:
            del p.parent.left
            del p.parent 
            del p
        elif p.data > p.parent.data:
            del p.parent.right
            del p.parent
            del p

    if not hasattr(p, 'right'):
        if not hasattr(p, 'left'):
            free(p)
        else:#how to solve?
            if p.data < p.parent.data:
                p.parent.left = p.left
                p.left.parent = p.parent
                del p
            elif p.data > p.parent.data:
                p.parent.right = p.left
                p.left.parent = p.parent
                del p
    else:
        if not hasattr(p, 'left'):
            q = p
            p = p.right
            del q
        else: #have both left and right child
            q = p
            s = p.left
            while hasattr(s, 'right'):
                q, s = s, s.right
            p.data = s.data
            if q != p:
                q.right = s.left
            else:
                q.left = s.left
            del s


def delete_bst(t, key):
    """
    `param t`:type BTree 
    `param key`: key
    """
    flag, p, pp = search_bst(t.root, key)   
    if flag == False:
        return

    delete(p)

if __name__ == '__main__':
    st = gen_sstable_seq()
    output_sstable(st)
    print("search_seq(st, 7)={}".format(search_seq(st, 7)))
    print("search_bin(st, 7)={}".format(search_seq(st, 7)))
    
    
    t = BTree()
    for i in xrange(random.randint(2**3, 2**5)):
        insert_bst(t, random.randint(2**4, 2**5))
    
    print("##############################################################")
    insert_bst(t, 999999)
    output_btree('', t.root)
    print("##############################################################")
    delete_bst(t, 999999)
    output_btree('', t.root)
