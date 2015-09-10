#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       huffmancode.py
Create Time:    2015-09-10 16:36
Description:
    huffmantree python impl.                
"""
import random

class HTNode(object):
    def __init__(self, weight, parent, lchild, rchild):
        self.weight = weight
        self.parent = parent
        self.lchild = lchild
        self.rchild = rchild


class HuffmanTree(object):
    def __init__(self, w, n):
        """
        `param w`: weight array, 0 unused
        `param n`: array length
        """
        assert len(w) == n + 1

        self.w = w
        self.n = n 
        self.nodes = [None]

        m = 2*n - 1
        for i in xrange(1, n+1):
            self.nodes.append(HTNode(w[i], 0, 0, 0))
        
        for i in xrange(n+1, m+1):
            self.nodes.append(HTNode(0, 0, 0, 0))

        
        for i in xrange(n+1, m+1):
            s1, s2 = self.select_min_two(i-1)
            self.nodes[s1].parent = i
            self.nodes[s2].parent = i
            self.nodes[i].lchild = s1
            self.nodes[i].rchild = s2
            self.nodes[i].weight = self.nodes[s1].weight + self.nodes[s2].weight
       
    def output(self):
        for i in xrange(1, self.n+1):
            cd = [0 for x in xrange(n)]
            start = n
            
            c = i
            f = self.nodes[i].parent
            
            while f != 0:
                start -= 1
                if self.nodes[f].lchild == c:
                    cd[start] = 0
                else:
                    cd[start] = 1
            
                c = f
                f = self.nodes[f].parent
            
            print "index:{} weight:{} code:{}".format(i, self.w[i], cd[start:]) 


    def select_min_two(self, idx):
        """
        `param idx`: max index, 1, 2, 3, .. i, 0 unused
        `return`: s1, s2, the two smallest num
        """
        sum = 0xffffffff
        idx1 = 0
        idx2 = 0
        for i in xrange(1, idx+1):
            if self.nodes[i].parent != 0:
                continue
            for j in xrange(i+1, idx+1):
                if self.nodes[j].parent != 0:
                    continue
                if self.nodes[i].weight + self.nodes[j].weight < sum:
                    sum = self.nodes[i].weight + self.nodes[j].weight
                    idx1 = i
                    idx2 = j
        return (idx1, idx2)


if __name__ == '__main__':
    #start here....
    for tt in xrange(10):
        print '##############################################################'
        w = [ random.randint(1, 100)  for j in xrange(random.randint(5, 10)) ]
        n = len(w)
        w.insert(0, 0) 

        ht = HuffmanTree(w, n)
        ht.output()
