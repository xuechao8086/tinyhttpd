#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include <iostream>
#include <string>

typedef struct list_head {
    struct list_head *next, *prev;
} list_head;

#define LIST_HEAD_INIT(name) {&(name), &(name)}
