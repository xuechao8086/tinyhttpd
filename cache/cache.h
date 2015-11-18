/*************************************************************************
> File Name: cache.h
> Author: charliezhao 
> Mail: 
> Created Time: Tue 17 Nov 2015 12:42:35 AM PST
************************************************************************/

#ifndef _CACHE_H
#define _CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include <iostream>
extern int cache_error;

typedef int cache_constructor_t(void *obj, void *notused1, void *notused2);

typedef void cache_destructor_t(void *obj, void *notused);

// note: no pre allocator right now.
typedef struct {
    pthread_mutex_t mutex;

    char *name;
    void **ptr;
    size_t bufsize;
    int freetotal;
    int freecurr;

    cache_constructor_t *constructor;
    cache_destructor_t *destructor;

} cache_t;

cache_t * cache_create(const char *name, size_t bufsize, size_t align, cache_constructor_t *construct, cache_destructor_t *destructor);
void cache_destroy(cache_t *cache);
void *cache_alloc(cache_t *cache);
void cache_free(cache_t *cache, void *ptr);

#endif
