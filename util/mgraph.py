#!/usr/bin/env python2.7
#coding:utf-8
"""
Author:         charliezhao
Filename:       mgraph.py
Create Time:    2015-09-12 11:11
Description:
    data graph
"""
from __future__ import print_function
import random
import Queue
import copy

class ArcCell(object):
    def __init__(self, adj, info=""):
        self.adj = adj
        self.info = info


class VertexType(object):
    def __init__(self, pos):
        self.pos = pos


class MGraph(object):
    def __init__(self, vexs, arcs, vexnum, arcnum, kind="UDG"):
        self.vexs = vexs
        self.arcs = arcs
        self.vexnum = vexnum
        self.arcnum = arcnum
        self.kind = kind


INFINITY = 2**20 


def create_udn(vexnum, arcnum):
    vexs = [VertexType(i) for i in xrange(vexnum)]
    arcs = [[ArcCell(INFINITY, None) for j in xrange(vexnum)]\
            for i in xrange(vexnum)]
    
    #print("input i, j, weight #i = (0,1..{0}) j = (0, 1..{0}), weight = 0,1, max"\
    #      .format(vexnum-1, vexnum-1))

    for i in xrange(arcnum):
        #idx1, idx2, w = raw_input("$:").replace(','," ").split()
        #idx1 = int(idx1)
        #idx2 = int(idx2)
        #w = int(w)
        #if idx1==idx2:
        #    print("err: idx1:{} idx2:{}".format(idx1, idx2)) 
        while True:
            idx1, idx2, w = gen_arcs(vexnum)
            if arcs[idx1][idx2] != INFINITY:
                break
        arcs[idx1][idx2].adj = w
        arcs[idx2][idx1].adj = w 

    return  MGraph(vexs, arcs, vexnum, arcnum)


def create_dn(vexnum, arcnum):
    vexs = [VertexType(i) for i in xrange(vexnum)]
    arcs = [[ArcCell(INFINITY, None) for j in xrange(vexnum)]\
            for i in xrange(vexnum)]
    
    for i in xrange(arcnum):
        while True:
            idx1, idx2, w = gen_arcs(vexnum)
            if arcs[idx1][idx2] != INFINITY:
                break
        arcs[idx1][idx2].adj = w
    return MGraph(vexs, arcs, vexnum, arcnum) 


def create_dn_for_critical_path():
    vexnum = 6
    vexs = [VertexType(i) for i in xrange(vexnum)]
   
    arcs = [[ArcCell(INFINITY, None) for j in xrange(vexnum)]\
            for i in xrange(vexnum)]
 
    _arcs = [(0, 1, 3),
             (0, 2, 2),
             (1, 3, 2),
             (1, 4, 3),
             (2, 3, 4),
             (2, 5, 3),
             (3, 5, 2),
             (4, 5, 1)]
    for i, j, w in _arcs:
        arcs[i][j].adj = w
    return MGraph(vexs, arcs, vexnum, len(_arcs))


def output_udn(mg):
    def output_header():
        print("#####################################################################")
        s = "       "
        for i in xrange(mg.vexnum):
            s = "{0}{1:>4}".format(s, i)
        print(s)
        s = "       "
        for i in xrange(mg.vexnum):
            s = "{0}{1:>4}".format(s, '^')
        print(s)
    
    output_header()

    for i in xrange(mg.vexnum):
        s = "{:3d}|   ".format(i)
        for j in xrange(mg.vexnum):
            if mg.arcs[i][j].adj == INFINITY:
                s = "{0}{1:>4}".format(s, "-")
            else:
                s = "{0}{1:>4}".format(s, mg.arcs[i][j].adj)
        print(s)
    
    print("#####################################################################")

def gen_arcs(vexnum):
    while True:
        idx1 = random.randint(0, vexnum-1)
        idx2 = random.randint(0, vexnum-1)
        if idx1 == idx2:
            continue
        else:
            w = random.randint(1, 9)
            break
    
    return idx1, idx2, w


def dfs_traverse(mg):
    def dfs(v):
        def first_adj_vex(v):
            for i in xrange(0, mg.vexnum):
                if mg.arcs[v][i].adj != INFINITY:
                    return i
            return -1
       
        def next_adj_vex(v, w):
            for i in xrange(w, mg.vexnum):
                if visited[i]:
                    continue
                if mg.arcs[v][i].adj != INFINITY:
                    return i
            return -1
        
        visited[v] = True
        order.append(v)

        w = first_adj_vex(v)
        while True:
            if w<0:
                break
            if not visited[w]:
                dfs(w) 
            w = next_adj_vex(v, w) 

    visited = [False for i in xrange(mg.vexnum)]
    order = [] 

    for v in xrange(0, mg.vexnum):
        v = 0 #onlu one node, for test!
        if not visited[v]:
            dfs(v)
    return order


def bfs_traverse(mg):
    visited = [False for i in xrange(mg.vexnum)]
    q = Queue.Queue()
    result = [] 
    #for v in xrange(mg.vexnum):
    for v in xrange(1):
        if not visited[v]:
            visited[v] = True
            result.append(v)
            q.put(v)
        while not q.empty():
            u = q.get()
            for j in xrange(mg.vexnum):
                if mg.arcs[u][j].adj == INFINITY:
                    continue
                if not visited[j]:
                    visited[j] = True
                    result.append(j)
                    q.put(j)
    return result


class CloseEdge(object):
    def __init__(self, adjvex, lowcost):
        self.adjvex = adjvex
        self.lowcost = lowcost


def mini_span_tree_prim(mg, u):
    """
    u: pos1 
    """
    U = [u]
    V = [i for i in xrange(mg.vexnum) if i != u]

    close_edge = [None for i in xrange(mg.vexnum)]
    for i in xrange(mg.vexnum):
        if i != u:
            w = mg.arcs[u][i].adj
            close_edge[i] = CloseEdge(u, w)
    close_edge[u] = CloseEdge(u, 0)
    
    def minimum():
        smallest = 2**10
        idx = -1
        for i in V:
            if not i in U:
                if smallest > close_edge[i].lowcost > 0:
                    smallest = close_edge[i].lowcost
                    idx = i
        return idx
    
    O = [] 
    for i in xrange(mg.vexnum):
        k = minimum()
        if k == -1:
            break
        #print("{:>2d} -> {:2d}".format(close_edge[k].adjvex, k))
        O.append((close_edge[k].adjvex, k))
        close_edge[k].lowcost = 0
        U.append(k)
        V.remove(k)
        for j in xrange(mg.vexnum):
            w = mg.arcs[k][j].adj
            if mg.arcs[k][j].adj < close_edge[j].lowcost:
                close_edge[j].adjvex = k
                close_edge[j].lowcost = mg.arcs[k][j].adj

    print("Prim Minimum Cost Spanning Tree, {}:{}"\
            .format(mg.vexnum, len(O)))
    cost = 0 
    for idx1, idx2 in sorted(O, key=lambda d:d[0], reverse=False):
        cost += mg.arcs[idx1][idx2].adj
        print("{:>2d} -> {:2d} cost:{}".format(idx1, idx2, cost))


def mini_span_tree_kruskal(mg, u):
    """
    has bug, need fix!
    """
    T = [[i] for i in xrange(mg.vexnum)]
    F = [ ]

    def minimum():
        smallest = 2**10
        idx1, idx2 = -1, -1 
        val1, val2 = -1, -1
        for i in xrange(len(T)):
            for j in xrange(i+1, len(T)):
                if len(T[j]) == 0:
                    continue
                for v in T[i]:
                    for u in T[j]:
                        w = mg.arcs[v][u].adj
                        if smallest > w > 0:
                            smallest = w
                            idx1, idx2 = i, j
                            val1, val2 = v, u
        T[idx1].append(val2) 
        T[idx2].remove(val2) 
        #print(T)
        return (val1, val2)
    

    R = [] 
    O = [] #for output

    while len(R) != mg.vexnum:
        idx1, idx2 = minimum()
        if idx1 in R and idx2 in R:
            continue
        if idx1 not in R: 
            R.append(idx1)
        if idx2 not in R:
            R.append(idx2)
            
        #print("{:>2d} -> {:2d}".format(idx1, idx2))
        O.append((idx1, idx2))
    
    print("Kruskal Minium Cost Spanning Tree, {}:{}"\
            .format(mg.vexnum, len(O)))
    
    cost = 0
    for idx1, idx2 in sorted(O, key=lambda d:d[0], reverse=False):
        cost += mg.arcs[idx1][idx2].adj
        print("{:>2d} -> {:2d} cost:{:>3d} ".format(idx1, idx2, cost))


def topological_sort(mg):
    """
    used for direct graph
    return True if have no cycle
    return False if have cycle
    """
    indegree = [0 for i in xrange(mg.vexnum)] 
    ve = [0 for i in xrange(mg.vexnum)] #for critical path
    for i in xrange(mg.vexnum):
        c = 0
        for j in xrange(mg.vexnum):
            if mg.arcs[j][i].adj != INFINITY:
                c += 1
        indegree[i] = c
    
    stack = []
    for i in xrange(mg.vexnum):
        if not indegree[i]:
            stack.append(i)
    O = []
    while len(stack):
        i = stack.pop()
        O.append(i)
        for j in xrange(mg.vexnum):
            if mg.arcs[i][j].adj != INFINITY:
                indegree[j] -= 1
                if not indegree[j]:
                    stack.append(j)
            if mg.arcs[i][j].adj!=INFINITY and\
                   ve[i] + mg.arcs[i][j].adj > ve[j]:
                ve[j] = ve[i] + mg.arcs[i][j].adj
    
    flag = True if len(O) == mg.vexnum else False
    print("Topological Sort:", flag)
    print(O)
    return (flag, ve, O)


def critical_path(mg):
    flag, ve, stack = topological_sort(mg)
    if flag == False:
        return flag
    vl = copy.deepcopy(ve)
    
    while len(stack):
        j = stack.pop()
        for k in xrange(mg.vexnum):
            dut = mg.arcs[j][k].adj
            if dut == INFINITY:
                continue
            if vl[k] - dut < vl[j]:
                vl[j] = vl[k] - dut


    print("Critical Path:")
    print("{:>3s}{:>3s}{:>4s}{:>3s}{:>3s}{:>4s}".format('j', 'k', 'dut', 'ee', 'el', 'tag'))  
    for j in xrange(mg.vexnum):
        for k in xrange(mg.vexnum):
            if mg.arcs[j][k].adj == INFINITY:
                continue
            dut = mg.arcs[j][k].adj
            ee, el = ve[j], vl[k] - dut
            tag = '*' if ee == el else '-'
            print("{:>3d}{:>3d}{:>4d}{:>3d}{:>3d}{:>4s}".format(j, k, dut, ee, el, tag))

    print("#####################################################################") 
    def output_ve_vl():
        print("vertex  ve vl")
        for i in xrange(0, mg.vexnum):
            print("{:>5}{:>4}{:>4}".format("v{}".format(i+1), ve[i], vl[i]))
    output_ve_vl()


if __name__ == '__main__':
    vexnum = random.randint(3, 9)
    arcnum = vexnum*(vexnum-1)/2
    arcnum = random.randint(vexnum*(vexnum-1)/2, vexnum*(vexnum-1))
    arcnum = random.randint(1, vexnum*(vexnum-1)/2)
    mg = create_dn_for_critical_path()
    output_udn(mg)
    
    order = dfs_traverse(mg)
    print("dfs={}, {}:{}".format(order, mg.vexnum, len(order)))
    order = bfs_traverse(mg) 
    print("bfs={}, {}:{}".format(order, mg.vexnum, len(order)))
    
    mini_span_tree_prim(mg, 0)
    #mini_span_tree_kruskal(mg, 0)
    topological_sort(mg)
    critical_path(mg)
    
