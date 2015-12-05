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

#define MAXEVENTS 1024


int startup(int port);
int get_line(int sock, char *buf, int size);
int get_line2(int sock, char *buf, int size);
int accept_request(int client);

#endif
