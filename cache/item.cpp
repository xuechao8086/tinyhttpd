/*************************************************************************
	> File Name: item.cpp
	> Author: charliezhao 
	> Mail: 
	> Created Time: Tue 17 Nov 2015 08:35:24 PM PST
 ************************************************************************/
#include "memcache.h"
extern struct settings settings;

static item *head[LARGEST_ID];
static item *tail[LARGEST_ID];

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
                    const rel_time_t exptime, const int nbytes,
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
    it->nkey = nkey;
    it->nbytes = nbytes;
    memcpy(ITEM_key(it), key, nkey);
    it->exptime = exptime;
    memcpy(ITEM_suffix(it), suffix, (size_t)nsuffix);
    it->nsuffix = nsuffix;
    return it;
}

void item_free(item *it) {
    size_t ntotal = ITEM_ntotal(it);
    unsigned int clsid = slabs_clsid(ntotal);
    slabs_free((void *)it, ntotal, clsid);
}

#ifdef ITEM_TEST
int main(int argc, char **argv) {
    settings.use_cas = true;
    return 0;
}
#endif
