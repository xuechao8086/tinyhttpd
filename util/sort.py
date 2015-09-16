#!/usr/bin/env python2.7
#coding:utf8

"""
Author:         charliezhao
Filename:       heapsort.py
Create Time:    2015-09-09 18:47
Description:
    all kinds of sort                
"""
from __future__ import print_function
import random

def LT(a, b):
    return a<b

def GT(a, b):
    return a>b

def EQ(a, b):
    return a==b

def LQ(a, b):
    return a<=b

def GQ(a, b):
    return a>=b

class SqList(object):
    def __init__(self, length, a=0, b=2**6):
        self.r = [random.randint(a, b)
                    for i in xrange(length+1)]
        self.length = length
        self.r[0] = 0 

    def output(self, prefix=""):
        print("{}arr = {}".format(prefix, self.r[1:]))


#####################################################
#heap sort
class HeapType(object):
    def __init__(self, length, a=0, b=100):
        self.arr = [random.randint(a, b) 
                    for i in xrange(length) ]
     
        self.length = length


def heapadjust(ht, s, m):
    rc = ht.arr[s]
    
    j = 2*s
    while j <= m:
        if j < m and ht.arr[j] < ht.arr[j+1]:
            j = j+1
        if not rc < ht.arr[j]:
            break
        ht.arr[s] = ht.arr[j]
        s = j

        j *= 2

    ht.arr[s] = rc


def heapsort(ht):
    for i in xrange(ht.length/2-1, 0, -1):
        heapadjust(ht, i, ht.length-1)

    for i in xrange(ht.length-1, 0, -1):
        ht.arr[0], ht.arr[i] = ht.arr[i], ht.arr[1] 
        heapadjust(ht, 0, i-1)

#####################################################
#quick sort
def insertsort(sl):
    for i in xrange(2, sl.length+1):
        if LT(sl.r[i], sl.r[i-1]):
            sl.r[0] = sl.r[i]
            sl.r[i] = sl.r[i-1]
        
        j = i - 2
        while LT(sl.r[0], sl.r[j]):
            sl.r[j+1] = sl.r[j]
            j -= 1
        sl.r[j+1] = sl.r[0]


def binsertsort(l):
    for i in xrange(2, l.length+1):
        l.r[0] = l.r[i]
        low, high = 1, i-1
        while low <= high:
            m = (low+high)/2
            if LT(l.r[0], l.r[m]):
                high = m - 1
            else:
                low = m + 1
        
        for j in xrange(i-1, high, -1):
            l.r[j+1] = l.r[j]
        l.r[high+1] = l.r[0]


def shellinsert(l, dk):
    for i in xrange(dk+1, l.length+1, 1):
        if LT(l.r[i], l.r[i-dk]):
            l.r[0] = l.r[i]
        
        j = i - dk
        while j>0 and LT(l.r[0], l.r[j]):
            l.r[j+dk] = l.r[j]
            j -= dk
        l.r[j+dk] = l.r[0]
    

def shellsort(l, dlta):
    """
    `param l`: list
    `param dlta`: list, increment seq.
    """
    for i in dlta:
        shellinsert(l, i)


#####################################################
#quick sort
def partition(l, low, high):
    l.r[0] = l.r[low]
    pivotkey = l.r[low]

    while low<high:
        while low<high and GQ(l.r[high], pivotkey):
            high -= 1
        l.r[low] = l.r[high]

        while low<high and LQ(l.r[low], pivotkey):
            low += 1
        l.r[high] = l.r[low]
    l.r[low] = l.r[0]
    return low 
    
def qsort(l, low, high):
    if(low<high):
        pivotloc = partition(l, low, high)
        qsort(l, low, pivotloc-1)
        qsort(l, pivotloc+1, high)

def quicksort(l):
    qsort(l, 1, l.length)


#####################################################
#merging sort
#http://www.cnblogs.com/Jedimaster/archive/2013/11/07/3412746.html
import heapq
import random
import threading

# k threads
k = 4

# Generate n numbers for each thread to process.
n = 10

def GenerateNumbers(a, b):
    numbers = []
    for i in xrange(n):
        numbers.append(random.randint(a, b))
    return numbers

class SortingThread(threading.Thread):
    def __init__(self):
        super(SortingThread, self).__init__()
        self.numbers = GenerateNumbers(1, 100)

    def run(self):
        self.numbers.sort()
        
if __name__ == '__main__':
    # Parallel sort with multiple threads.
    #
    sortingThreads = []
    for i in xrange(k):
        sortingThreads.append(SortingThread())
    for i in xrange(k):
        sortingThreads[i].start()
    for i in xrange(k):
        sortingThreads[i].join()
        print(i, sortingThreads[i].numbers)

    # Use heap to output the final sorted list.
    # We fill the heap with several numbers firstly
    #
    theHeap = []
    for i in xrange(1):
        for t in xrange(len(sortingThreads)):
            number = sortingThreads[t].numbers[i]
            heapq.heappush(theHeap, number)

    sortedNumbers = []

    # In the while loop, people may replace the code by reading file routines.
    # Since IO is always the bottleneck, so single thread should be enough.
    p = 1
    while (len(theHeap) > 0):
        if p < len(sortingThreads[t].numbers):
            for t in xrange(len(sortingThreads)):
                number = sortingThreads[t].numbers[p]
                heapq.heappush(theHeap, number)
            p += 1
        sortedNumbers.append(heapq.heappop(theHeap))

    # Done
    print(sortedNumbers)


#####################################################
#merging sort2
def megre_sort(A, p, r):
    if p < r:
        q = (p + r) // 2
        megre_sort(A, p, q)
        megre_sort(A, q + 1, r)
        megre(A, p, q, r)
        return A

def megre(A, p, q, r):
    n1 = q - p + 1
    n2 = r - q

    L = [None] *n1
    R = [None] *n2
    # print A
    for i in range(n1):
       L[i] = A[i + p]
    L.append(float("inf"))
    for j in range(n2):
        R[j] = A[q + j + 1]
    R.append(float("inf"))

    i = j = 0;
    # print L
    # print R
    for k in range(p, r + 1):
        if L[i] < R[j]:
            A[k] = L[i]
            i += 1
        else:
            A[k] = R[j]
            j += 1
    return A


#####################################################
#radix sort
def radixsort(l, scale):
    def LT(a, b):
        return a//scale%10 < b//scale%10
    def GT(a, b):
        return a//scale%10 > b//scale%10

    for i in xrange(2, l.length+1):
        l.r[0] = l.r[i]
        low, high = 1, i-1
        while low <= high:
            m = (low+high)/2
            if LT(l.r[0], l.r[m]):
                high = m - 1
            else:
                low = m + 1
        
        for j in xrange(i-1, high, -1):
            l.r[j+1] = l.r[j]
        l.r[high+1] = l.r[0]



if __name__ == '__main__':
    print("heap sort:") 
    for i in xrange(10): 
        ht = HeapType(random.randint(2**2, 2**4)) 
        heapsort(ht)
        print(ht.arr)
    print("insert sort:") 
    for i in xrange(10): 
        sl = SqList(random.randint(2**2, 2**4))
        insertsort(sl)
        sl.output()
    
    print("binsert sort:") 
    for i in xrange(10): 
        sl = SqList(random.randint(2**2, 2**4))
        binsertsort(sl)
        sl.output()
    
    print("shell sort:, have bug, need fix!") 
    dlta = [ i for i in xrange(3, 1, -1) ]
    dlta = [ 1 ]
    for i in xrange(10): 
        sl = SqList(random.randint(2**2, 2**4))
        sl.output("~")
        shellsort(sl, dlta) 
        sl.output("*")
    
    print("quick sort:")
    for i in xrange(10): 
        sl = SqList(random.randint(2**2, 2**4))
        quicksort(sl) 
        sl.output()
    
    print("merge sort:")
    for i in xrange(10):
        A = [ random.randint(2**3, 2**4) for i in xrange(0, random.randint(2**3, 2**4)) ]
        print(megre_sort(A, 0, len(A) -1))
    
    print("redix sort:")
    for i in xrange(10):
        sl = SqList(random.randint(2**2, 2**4), 2**4, 2**10)
        for j in (1, 10, 100): 
            radixsort(sl, j)
        sl.output()
