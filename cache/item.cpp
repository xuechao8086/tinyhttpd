/*************************************************************************
	> File Name: item.cpp
	> Author: charliezhao 
	> Mail: 
	> Created Time: Tue 17 Nov 2015 08:35:24 PM PST
 ************************************************************************/
#include "memcache.h"

extern struct settings settings;
extern time_t current_time;

static item *heads[LARGEST_ID]; // default NULL 
static item *tails[LARGEST_ID]; // default NULL

static pthread_mutex_t cas_id_lock = PTHREAD_MUTEX_INITIALIZER;

uint64_t get_cas_id(void) {
    static uint64_t cas_id = 0;
    pthread_mutex_lock(&cas_id_lock); 
    uint64_t next_id = ++cas_id;
    pthread_mutex_unlock(&cas_id_lock);
    return next_id;
}

static size_t item_make_header(const uint8_t nkey, const int flags, const int nbytes, char *suffix, uint8_t *nsuffix)
{
    *nsuffix = (uint8_t)snprintf(suffix, 40, "%d %d\r\n", flags, nbytes);
    return sizeof(item)+nkey+*nsuffix+nbytes;
}



item *do_item_alloc(char *key, const size_t nkey, const int flags,
                    const time_t exptime, const int nbytes,
                    const uint32_t cur_hv) 
{
    uint8_t nsuffix;

    char suffix[40];
    size_t ntotal = item_make_header(nkey+1, flags, nbytes, suffix, &nsuffix);
    
    if(settings.use_cas) {
        ntotal += sizeof(uint64_t);
    }
    
    unsigned int id = slabs_clsid(ntotal);
    if(id == 0) {
        return NULL;
    }
    
    unsigned int total_chunks;
    item *it = (item *)slabs_alloc(ntotal, id, &total_chunks);
    if(it == NULL) {
        return NULL;
    }
    
    it->slabs_clsid = COLD_LRU;
    it->it_flags = settings.use_cas? ITEM_CAS : 0;
    it->it_flags &= ~ITEM_SLABBED;
    it->nkey = nkey;
    it->nbytes = nbytes;
    memcpy(ITEM_key(it), key, nkey);
    it->exptime = exptime;
    memcpy(ITEM_suffix(it), suffix, (size_t)nsuffix);
    it->nsuffix = nsuffix;
    
    do_item_link(it, cur_hv);

    return it;
}

void item_free(item *it) {
    size_t ntotal = ITEM_ntotal(it);
    unsigned int clsid = slabs_clsid(ntotal);
    slabs_free((void *)it, ntotal, clsid);
}

int do_item_link(item *it, const uint32_t hv) {
    assert((it->it_flags & (ITEM_LINKED|ITEM_SLABBED)) == 0);    
    
    it->it_flags |= ITEM_LINKED;
    it->time = current_time;
    
    ITEM_set_cas(it, (settings.use_cas)?get_cas_id():0);
    assoc_insert(it, hv);
    item_link(it);
    ++it->refcount;
}

void item_link(item *it) {
    item **head = &heads[it->slabs_clsid];
    item **tail = &tails[it->slabs_clsid];

    assert(it != *head);
    assert((*head && *tail) || (*head == NULL && *tail == NULL));

    it->prev = NULL;
    it->next = *head;
    *head = it;
    if(*tail == NULL) {
        *tail = it;
    }
}

void do_item_unlink(item *it, const uint32_t hv) {
    if((it->it_flags & ITEM_LINKED) != 0) {
        it->it_flags &= ~ITEM_LINKED;
        assoc_delete(ITEM_key(it), it->nkey, hv);
        item_unlink(it);
        do_item_remove(it);
    }
}

void item_unlink(item *it) {
    item **head = &heads[it->slabs_clsid];
    item **tail = &tails[it->slabs_clsid];

    if(*head == it) {
       assert(it->prev == NULL);
        *head = it->next;
    }
    if(*tail == it) {
        assert(it->next == NULL);
        *tail = it->prev;
    }

    assert(it->next != it);
    assert(it->prev != it);
    
    if(it->next) {
        it->next->prev = it->prev;
    }
    if(it->prev) {
        it->prev->next = it->next;
    }
}

void do_item_remove(item *it) {
    assert((it->it_flags & ITEM_SLABBED) == 0);
    assert(it->refcount > 0);

    if(--it->refcount == 0) {
        item_free(it);
    }
}


#ifdef ITEM_TEST
int main(int argc, char **argv) {
    settings.use_cas = true;
    return 0;
}
#endif
