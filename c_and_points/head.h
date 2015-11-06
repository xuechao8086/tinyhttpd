#ifndef HEAD_H
#define HEAD_H
// for c lib
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <float.h>

// for linux env 
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <limits.h>

//for heapsort
#include <bsd/stdlib.h>

// for shm, system v
#include <sys/ipc.h>
#include <sys/shm.h>

// for posix shm
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

// for posix aio
#include <aio.h>

// for socket
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// for TCP_NODELAY
#include <netinet/tcp.h> 

// for c++ lib
#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <mutex>
#include <queue>
#include <thread>
#include <condition_variable>
#include <functional>
#include <iomanip>

#define MAXFD 64

typedef struct list_head {
    struct list_head *next, *prev;
} list_head;

#define LIST_HEAD_INIT(name) {&(name), &(name)}

struct s_args {
    uint32_t i;
    uint32_t j;
    uint32_t k;
    uint32_t m;
};


namespace util {
    //int daemon(const char *pname, int facility);
    int daemon();
    void sig_chld(int signo);
};

#endif
