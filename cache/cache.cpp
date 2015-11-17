/*************************************************************************
  > File Name: cache.cpp
  > Author: charliezhao 
  > Mail: 
  > Created Time: Tue 17 Nov 2015 01:05:06 AM PST
 ************************************************************************/

#include "cache.h"

const uint64_t redzone_pattern = 0xdeadbeefcafebabe;
int cache_error = 0;

const int initial_pool_size = 64;

cache_t* cache_create(const char *name, size_t bufsize, size_t align,
        cache_constructor_t *constructor,
        cache_destructor_t *destructor) {

    cache_t *ret = (cache_t *)calloc(1, sizeof(cache_t));
    char *nm = strdup(name);
    void **ptr = (void **)calloc(initial_pool_size, sizeof(void *));
    if(ret == NULL || nm == NULL || ptr == NULL ||
            pthread_mutex_init(&ret->mutex, NULL) == -1) {
        free(ret);
        free(nm);
        free(ptr);
        return NULL;
    } 

    ret->name = nm;
    ret->ptr = ptr;
    ret->freetotal = initial_pool_size;
    ret->constructor = constructor;
    ret->destructor = destructor;

    ret->bufsize = bufsize + 2*sizeof(redzone_pattern);
    return ret;
}

static inline void* get_object(void *ptr) {
    uint64_t *pre = (uint64_t *)ptr;
    return pre+1;
}

void cache_destroy(cache_t *cache) {
    while(cache->freecurr > 0) {
        void *ptr = cache->ptr[--cache->freecurr];
        if(cache->destructor) {
            cache->destructor(get_object(ptr), NULL);
        }
        free(ptr);
    }
    free(cache->name);
    free(cache->ptr);
    pthread_mutex_destroy(&cache->mutex);
    free(cache);
}

void * cache_alloc(cache_t *cache) {
    void *ret = NULL;
    void *object = NULL;
    pthread_mutex_lock(&cache->mutex);
    if(cache->freecurr > 0) {
        ret = cache->ptr[--cache->freecurr];
        object = get_object(ret);
    }
    else  {
        object = ret = malloc(cache->bufsize);
        if(ret != NULL) {
            object = get_object(ret);

            if(cache->constructor != NULL &&
                    cache->constructor(object, NULL, 0) != 0) {
                free(ret);
                object = NULL;
            }
        }
    }
    pthread_mutex_unlock(&cache->mutex);

    if(object != NULL) {
        uint64_t *pre = (uint64_t *)ret;
        *pre = redzone_pattern;
        ret +=  1;
        memcpy(((char *)ret)+cache->bufsize-(2*sizeof(redzone_pattern)),
                &redzone_pattern, sizeof(redzone_pattern));
    }
    return object;
}

void cache_free(cache_t *cache, void *ptr) {
    pthread_mutex_lock(&cache->mutex);
    if (memcmp(((char*)ptr) + cache->bufsize - (2*sizeof(redzone_pattern)), 
                &redzone_pattern, sizeof(redzone_pattern)) != 0) {
        raise(SIGABRT);
        cache_error = 1;
        pthread_mutex_unlock(&cache->mutex);
        return;
    }

    uint64_t *pre = (uint64_t *)ptr;
    --pre;
    if (*pre != redzone_pattern) {
        raise(SIGABRT);
        cache_error = -1;
        pthread_mutex_unlock(&cache->mutex);
        return;
    }

    ptr = pre;
    if (cache->freecurr < cache->freetotal) {
        cache->ptr[cache->freecurr++] = ptr;
    } else {
        /* try to enlarge free connections array */
        size_t newtotal = cache->freetotal * 2;
        void **new_free = (void **)realloc(cache->ptr, sizeof(char *) * newtotal);
        if (new_free) {
            cache->freetotal = newtotal;
            cache->ptr = new_free;
            cache->ptr[cache->freecurr++] = ptr;
        } else {
            if (cache->destructor) {
                cache->destructor(ptr, NULL);
            }
            free(ptr);
        }
    }
    pthread_mutex_unlock(&cache->mutex);
}

int main(int argc, char **argv) {
    cache_t *ret = cache_create("charliezhao", 1024, 0, NULL, NULL);    
    std::cout<<"ret->name = "<<ret->name;
    std::cout<<std::endl;
    std::cout<<std::flush;

    void *ptr = cache_alloc(ret);
    memcpy(ptr, (const void *)"hello, from charlie", 100);
    std::cout<<"ptr = "<<(char *)ptr;
    std::cout<<std::endl;
    std::cout<<std::flush;
    return 0;
}
