#!/usr/bin/env python2.7
#coding:utf-8

"""
Author:         charliezhao
Filename:       recv.py
Create Time:    2015-07-07 22:39
Description:
    mail recv for QQMail and !26.
"""
import argparse

import imaplib
import string
import email

import os

SERVER = "imap.qq.com"
PORT = 993

def recv_mail(user, password):
    m = imaplib.IMAP4_SSL(SERVER, PORT)
    t = 0
    try:
        try:
            m.login(user, password)
        except Exception, e:
            print 'login error: %s'% e
            m.close()
        
        m.select('INBOX')
    
        typ, data = m.search(None, 'ALL')
        for num in string.split(data[0]):
            try:
                typ, data = m.fetch(num, '(UID, BODY.PEEK[])')
                msg = email.message_from_string(data[0][1])
                t += 1
                filename = str(t) + ".eml"
                f = open(filename, 'wb')
                f.write(str(msg))
                f.close()
            except Exception, e:
                print 'got msg error: %s'% e
        print "OK"
        m.logout()
    except Exception, e:
        print 'imap error: %s'% e
        m.close()


def parse_argument():
    parser = argparse.ArgumentParser(
        description="receive mail tool, default user xuechao8086@qq.com",
        epilog="""example: %(prog)s\
                -u xuechao8086@qq.com\
                -p user's password\
                """)

    parser.add_argument(
        "-u", "--user",
        type=str, default="xuechao8086@qq.com",
        metavar="user",
        help="xuechao8086@qq.com default sender")

    parser.add_argument(
        "-p", "--password",
        type=str, metavar="password",
        help="password", required=True)
    
    return parser.parse_args()


if __name__ == '__main__':
    args = parse_argument() 
    users, password = args.user, args.password
    recv_mail(users, password)
