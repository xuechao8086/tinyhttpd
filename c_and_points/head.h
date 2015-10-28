#ifndef HEAD_H
#define HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <float.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
