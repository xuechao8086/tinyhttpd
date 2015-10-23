#!/usr/bin/env python2.7 
#coding:utf-8 
"""
Author:         charliezhao
Filename:       placequeen.py
Create Time:    2015-10-21 18:54
Description:
    matrix queen                
"""
import sys
import pprint


class MatrixQueue(object):
    def __init__(self, size):
        self.board = [ [False for i in xrange(size)] \
                for j in xrange(size)]
        self.size = size 
        self.n_solutions = 0 

    def conflicts(self, row, column):
        
        rtn = False
        
        if reduce(lambda x, y : x or y, 
            (self.board[i][column] for i in xrange(self.size) if i < row),
            False): # upper
            rtn = True 
        
        if reduce(lambda x, y: x or y,
            (self.board[row][i] for i in xrange(self.size) if i != column),
            False): #left right
            rtn = True
        

        if reduce(lambda x, y: x or y,
                (self.board[row-i][column-i] for i in xrange(1, self.size) if (row - i >= 0 and column - i > 0)),
                False): # left diagonal
            rtn = True

        if reduce(lambda x, y: x or y,
                (self.board[row-i][column+i] for i in xrange(1, self.size) if row - i >= 0 and column + i < self.size),
                False): # right diagonal
            rtn = True
        
        #print("row =", row, "column=", column, "rtn=", rtn)
        #pprint.pprint(self.board)

        return rtn
    
    def test_conficts(self):
        self.board = [[True, False, False, False, False, False, False, False],
                [False, False, False, True, False, False, False, False],
                [False, False, False, False, False, False, False, False],
                [False, False, False, False, False, False, False, False],
                [False, False, False, False, False, False, False, False],
                [False, False, False, False, False, False, False, False],
                [False, False, False, False, False, False, False, False],
                [False, False, False, False, False, False, False, False]]
        
        self.conflicts(1, 3)

    def place_queue(self, row):
        for column in xrange(self.size):
            self.board[row][column] = True
            if row == 0 or not self.conflicts(row, column):
                if row < self.size - 1:
                    self.place_queue(row+1)
                else:
                    self.print_board()

            self.board[row][column] = False
    
    def print_board(self):
        self.n_solutions += 1
        print("solutions = {}".format(self.n_solutions))
        
        for row in xrange(self.size):
            t = []
            for column in xrange(self.size):
                if self.board[row][column]:
                    t.append("Q")
                else:
                    t.append("-")
            print(" ".join(i for i in t))
        
        print("")

if __name__ == '__main__':
    mq = MatrixQueue(8)
    mq.place_queue(0)
