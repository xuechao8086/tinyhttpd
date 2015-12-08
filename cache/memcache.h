/*************************************************************************
	> File Name: memcached.h
	> Author: 
	> Mail: 
	> Created Time: Fri 20 Nov 2015 02:35:01 AM PST
 ************************************************************************/

#ifndef _MEMCACHED_H
#define _MEMCACHED_H

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "conf.h"
#include "item.h"
#include "slab.h"
#include "assoc.h"
#include "jenkins_hash.h"
#include "test.h"
#include "util.h"

#define MAXEVENTS 1024
#define DATA_BUFFER_SIZE 2048

typedef struct conn conn;
struct conn {
    int sfd;
    char   *rbuf;   /** buffer to read commands into */
    char   *rcurr;  /** but if we parsed some already, this is where we stopped */
    int    rsize;   /** total allocated size of rbuf */
    int    rbytes;  /** how much data, starting from rcur, do we have unparsed */

    char *wbuf;
    char *wcurr;
    int wsize;
    int wbytes;
    conn *next;
};

enum try_read_result {
    READ_DATA_RECEIVED,
    READ_NO_DATA_RECEIVED,
    READ_ERROR,            /** an error occurred (on the socket) (or client closed connection) */
    READ_MEMORY_ERROR      /** failed to allocate more memory */
};

#endif
