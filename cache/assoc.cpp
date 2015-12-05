/*************************************************************************
	> File Name: assoc.cpp
	> Author: charliezhao 
	> Mail: 
	> Created Time: Wed 02 Dec 2015 07:26:14 PM PST
 ************************************************************************/

#include "memcache.h"
unsigned int hashpower = HASHPOWER_DEFAULT;
static item** primary_hashtable = 0;

void assoc_init(const int hashpower_init) {
    if(hashpower_init) {
        hashpower = hashpower_init;
    }
    primary_hashtable = (item **)calloc(hashsize(hashpower), sizeof(void *));
    if(! primary_hashtable) {
        fprintf(stderr, "Failed to init hashtable.\n");
        return; 
    }
}

int assoc_insert(item *it, const uint32_t hv) {
    it->h_next = primary_hashtable[hv&hashmask(hashpower)];
    primary_hashtable[hv & hashmask(hashpower)] = it;
    return 1;
}

item *assoc_find(const char *key, const size_t nkey, const uint32_t hv) {
    item *ret = NULL; 
    item *it = primary_hashtable[hv&hashmask(hashpower)];
    while(it) {
        if((nkey == it->nkey) && (memcmp(key, ITEM_key(it), nkey) == 0)) {
            ret = it;
            break;
        }
        it = it->h_next;
    }
    return ret;
}

void assoc_delete(const char *key, const size_t nkey, const uint32_t hv) {
    item *it = primary_hashtable[hv&hashmask(hashpower)];
    
    item *prev = it;
    while(it) {
        if((nkey == it->nkey) && (memcmp(key, ITEM_key(it), nkey) == 0)) {
            if(prev==it) {
                prev = it->h_next; 
            }
            else {
                prev->h_next = it->h_next;
            }
            it->h_next = NULL;
            break;
        }
        prev = it;
        it = it->h_next;
    }
}
