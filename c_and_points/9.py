#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       dollars.py
Create Time:    2015-10-22 16:42
Description:
    dollars format                
"""
class Dollars(object):
    def __init__(self, src, dst):
        self.src = src
        self.dst = dst
        self.src_len = len(self.src)

    def format(self):
        self.dst = "${}.{}".format(self.format_int(), self.format_decimal())
        return self.dst 

    def format_int(self):
        if self.src_len < 3:
            return "0"
        else:
            result = ""
            i = self.src_len - 2
            while True:
                beg = i - 3 if i - 3 > 0 else 0
                if beg == 0:
                    result = "{}{}".format(self.src[beg:i], result)
                    break
                else:
                    result = ",{}{}".format(self.src[beg:i], result)
                i -= 3
            return result
    
    def format_decimal(self):
        if self.src_len == 0:
            return "00"
        elif self.src_len == 1:
            return "0{}".format(self.src)
        else:
            return "{}".format(self.src[-2:])


class StringFormat(object):
    def __init__(self, src, format):
        self.src = src
        self.src_len = len(src)
        self.format = format
        self.format_len = len(format)
        self.result = []

    def do(self):
        f = self.format_len - 1
        s = self.src_len - 1
        
        while s >=0:
            if self.format[f] == '#':
                self.result.append(self.src[s])
                s -= 1
                f -= 1
            elif self.format[f] == ',':
                self.result.append(',')
                f -= 1
            elif self.format[f] == '.':
                self.result.append('.')
                f -= 1
            else:
                assert False
        while f >= 0:
            if self.format[f] == '#':
                self.result.append('_')
            elif self.format[f] == '.':
                for k in xrange(len(self.result)-1, -1, -1):
                    if self.result[k] == '_':
                        self.result[k] = '0'
                    else:
                        break
                self.result.append('.')
                self.result.append('0')
            f -= 1 
        
        r = ""
        for i in self.result:
            r = "{}{}".format(i, r)
        return r

if __name__ == '__main__':
    for src in "", "1", "12", "123", "1234", "12345", "123456", "1234567", "12345678", "123456789":
        d = Dollars(dst="", src= src)
        print(d.format())
    
    for format, src in (
            ("#####", "12345"), 
            ("#####", "123"), 
            ("##,###", "1234"),
            ("##,###", "123"),
            ("##,###", "1234567"),
            ("#,###,###.##", "123456789"),
            ("#,###,###.##", "1234567"), 
            ("#,###,###.##", "123"),
            ("#,###,###.##", "1"),
            ("#####.#####", "1"),
            ):
        d = StringFormat(src, format)
        print(d.do())
