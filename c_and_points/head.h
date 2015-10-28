#ifndef HEAD_H
#define HEAD_H
//for c lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <float.h>
#include <pthread.h>

//for linux env 
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>

//for socket
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//for c++ lib
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>

typedef struct list_head {
    struct list_head *next, *prev;
} list_head;

#define LIST_HEAD_INIT(name) {&(name), &(name)}

#endif
