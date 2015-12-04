/*************************************************************************
	> File Name: memcached.h
	> Author: 
	> Mail: 
	> Created Time: Fri 20 Nov 2015 02:35:01 AM PST
 ************************************************************************/

#ifndef _MEMCACHED_H
#define _MEMCACHED_H

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/signal.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>

typedef unsigned int rel_time_t;

#include "conf.h"
#include "item.h"
#include "slab.h"
#include "assoc.h"
#include "jenkins_hash.h"
#include "test.h"
#endif
