#!/usr/bin/env python3.4
#openssl req -new -x509 -days 365 -nodes -out cert.pem -keyout key.pem

#coding:utf-8

import socket, ssl,time


context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
context.load_cert_chain(certfile="cert.pem", keyfile="key.pem")


bindsocket = socket.socket()
bindsocket.bind(('172.16.184.131', 10023))
bindsocket.listen(5)


def do_something(connstream, data):
    #print("data length:",len(data))


    return True


def deal_with_client(connstream):
    t_recv=0
    t_send=0
    n = 0
    t1=time.clock()
    data = connstream.recv(1024)
    t2=time.clock()
    print("receive time:",t2-t1)
    # empty data means the client is finished with us
    while data:
        if not do_something(connstream, data):
            # we'll assume do_something returns False
            # when we're finished with client
            break
        n = n + 1
        t1=time.clock()
        connstream.send(b'b'*1024)
        t2=time.clock()
        t_send += t2-t1
        print("send time:",t2-t1)
        t1=time.clock()
        data = connstream.recv(1024)
        t2=time.clock()
        t_recv +=t2-t1
        print("receive time:",t2-t1)
    print("avg send time:",t_send/n,"avg receive time:",t_recv/n)
    # finished with client




while True:
    newsocket, fromaddr = bindsocket.accept()
    connstream = context.wrap_socket(newsocket, server_side=True)
    try:
        deal_with_client(connstream)
    finally:
        connstream.shutdown(socket.SHUT_RDWR)
        connstream.close()

