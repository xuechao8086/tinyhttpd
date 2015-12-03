/*************************************************************************
	> File Name: slab.cpp
	> Author: charliezhao 
	> Mail: 
	> Created Time: Fri 20 Nov 2015 02:25:46 AM PST
 ************************************************************************/
#include "memcached.h"

static slabclass_t slabclass[MAX_NUMBER_OF_SLAB_CLASSES];
static size_t mem_limit = 0;
static size_t mem_malloced = 0;

static bool mem_limit_reached = false;
static int power_largest;

static void *mem_base = NULL;
static void *mem_current = NULL;
static size_t mem_avail = 0;

/* Access to the slab allocator is protected by this lock */
static pthread_mutex_t slabs_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t slabs_rebalance_lock = PTHREAD_MUTEX_INITIALIZER;

/* private alloc funcs */
static void slabs_preallocate (const unsigned int maxslabs);
static int do_slabs_newslab(const unsigned int id);
static int grow_slab_list (const unsigned int id);
static void *memory_allocate(size_t size);
static void split_slab_page_into_freelist(void *ptr, const unsigned int id);
static void do_slabs_free(void *ptr, const size_t size, unsigned int id);
static void *do_slabs_alloc(const size_t size, unsigned int id, unsigned int *total_chunks);

/* charliezhao add */
static void slabs_info(void);

extern struct settings settings;

unsigned int slabs_clsid(const size_t size) {
    int res = POWER_SMALLEST;
    if (size == 0) {
        return 0;
    }
    while (size > slabclass[res].size) {
        if (res++ == power_largest) {
            return 0;
        }
    }
    return res;
}


void slabs_init(const size_t limit, const double factor, const bool prealloc=true) {
    mem_limit = limit;
    if (prealloc) {
        mem_base = malloc(mem_limit);
        if (mem_base != NULL) {
            mem_current = mem_base;
            mem_avail = mem_limit;
        }
        else {
            fprintf(stderr, "Warning: Failed to allocate requested memory in"
                   " one large chunk.\nWill allocate in smaller chunks\n");
        }
    }
    
    memset(slabclass, 0, sizeof(slabclass));
    
    unsigned int size = sizeof(item) + settings.chunk_size;
    int i = POWER_SMALLEST - 1;
    while (++i < MAX_NUMBER_OF_SLAB_CLASSES-1 && size <= settings.item_size_max / factor) {
        if (size % CHUNK_ALIGN_BYTES) {
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        }

        slabclass[i].size = size;
        slabclass[i].perslab = settings.item_size_max / slabclass[i].size;
        size *= factor;
        if (settings.verbose > 1) {
            fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u \n",
                   i, slabclass[i].size, slabclass[i].perslab);
        }
    }
    
    power_largest = i;
    slabclass[power_largest].size = settings.item_size_max;
    slabclass[power_largest].perslab = 1;
    if (settings.verbose > 1) {
        fprintf(stderr, "slab class %3d: chunk size %9u perslab %7u\n",
               i, slabclass[i].size, slabclass[i].perslab);
    }
    
    if (prealloc) {
        slabs_preallocate(power_largest);
    }
}

static void slabs_info(void) {
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    for(int32_t i = 1; i <= power_largest; ++i) {
        // to be done 
        item *ptr = (item *) slabclass[i].slots;
        uint32_t cnt = 0;
        while(ptr != NULL) {
            ptr = ptr->next;
            ++cnt; 
        }
        if(settings.verbose > 1) {
            fprintf(stderr, "slab class %3d: item cnt: %7u\n",
                    i, cnt);
        }
    }        
}

static void slabs_preallocate(const unsigned int maxslabs) {
    unsigned int prealloc = 0;
    for (int i = POWER_SMALLEST; i < MAX_NUMBER_OF_SLAB_CLASSES; i++) {
        if (++prealloc > maxslabs) {
            return;
        }
        if (do_slabs_newslab(i) == 0) {
            fprintf(stderr, "Error while preallocating slab memory!\n"
                   "max memory must be at least %d megabytes.\n", power_largest);
            exit(1); 
        }
    }
}

static int do_slabs_newslab(const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    int len = settings.slab_reassign ? settings.item_size_max : p->size * p->perslab;
    
    if ((mem_limit && mem_malloced + len > mem_limit && p->slabs > 0)) {
        mem_limit_reached = true;
        return 0;
    }    
    
    void *ptr;
    if ((grow_slab_list(id) == 0) || ((ptr = memory_allocate((size_t)len)) == 0)) {
        return 0;
    }
    memset(ptr, 0, (size_t)len);
    split_slab_page_into_freelist(ptr, id);
    p->slab_list[p->slabs++] = ptr;
    mem_malloced += len;
    return 1;
}

static int grow_slab_list (const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    if (p->slabs == p->list_size) {
        size_t new_size =  (p->list_size != 0) ? p->list_size * 2 : 16;
        void **new_list = (void **)realloc(p->slab_list, new_size * sizeof(void *));
        if (new_list == 0) return 0;
        p->list_size = new_size;
        p->slab_list = new_list;
    }
    return 1;
}

static void *memory_allocate(size_t size) {
    void *ret;
    if (mem_base == NULL) {
        ret = malloc(size);
    } 
    else {
        ret = mem_current;
        if (size > mem_avail) {
            return NULL;
        }
        if (size % CHUNK_ALIGN_BYTES) {
            size += CHUNK_ALIGN_BYTES - (size % CHUNK_ALIGN_BYTES);
        }
        mem_current = ((char*)mem_current) + size;
        if (size < mem_avail) {
            mem_avail -= size;
        }
        else {
            mem_avail = 0;
        }
    }
    return ret;
}

static void split_slab_page_into_freelist(void *ptr, const unsigned int id) {
    slabclass_t *p = &slabclass[id];
    for (uint32_t x = 0; x < p->perslab; x++) {
        do_slabs_free(ptr, 0, id);
        ptr += p->size;
    }
}

static void do_slabs_free(void *ptr, const size_t size, unsigned int id) {
    assert(id >= POWER_SMALLEST && id <= power_largest);

    slabclass_t *p = &slabclass[id];
    
    item *it = (item *)ptr;
    
    it->nbytes = size;
    it->it_flags |= ITEM_SLABBED;

    it->next = (item *)p->slots;
    if (it->next) it->next->prev = it;
    p->slots = it;
    p->sl_curr++;
    p->requested -= size;
    return;
}

void *slabs_alloc(size_t size, unsigned int id, unsigned int *total_chunk) {
    void * ret = do_slabs_alloc(size, id, total_chunk);
    return ret;
}

static void *do_slabs_alloc(const size_t size, unsigned int id, unsigned int *total_chunks) {
    void *ret = NULL;
    if(id < POWER_SMALLEST || id > power_largest) {
        fprintf(stderr, "slabs_alloc fail\n");
        return NULL;
    }
    slabclass_t *p = &slabclass[id];
    *total_chunks = p->slabs * p->perslab; // not used right now by charliezhao
    if(p->sl_curr != 0) {
        item *it = (item *)p->slots;
        assert(it!=NULL);
        p->slots = it->next;
        if(it->next) {
            it->next->prev = NULL;
        }
        it->it_flags &= ~ITEM_SLABBED;
        ++it->refcount; 
        --p->sl_curr;
        ret = (void *)it;
    }
    if(ret) {
        p->requested += size;
    }
    return ret;
}

void slabs_free(void *ptr, size_t size, unsigned int id) {
    do_slabs_free(ptr, size, id); 
}

void slabs_alloc_test(void) {
    unsigned int total_chunk = 0;
    
    const char *key = "charliezhao";
    size_t nkey = strlen(key) + 1;
    item *ptr = (item *)slabs_alloc(1024, slabs_clsid(1024), &total_chunk);
    strcpy(ITEM_key(ptr), key);   
    ptr->nkey = nkey;
    strcpy(ITEM_data(ptr), "xuechaozhao");
    uint32_t hv = jenkins_hash(key, strlen(key));
    assoc_insert(ptr, hv);

    for(int i = 0; i <= 10922; ++i) {
        void *ptr = slabs_alloc(96, slabs_clsid(96), &total_chunk);
        if(ptr == NULL) {
            fprintf(stderr, "i: %7d slabs_alloc fail\n", 
                    i);
            break;
        }
        else {
            slabs_free(ptr, 96, slabs_clsid(96)); 
        }
    }

    item *ptr2 = assoc_find(key, nkey, hv);
    fprintf(stdout, "key:%20s value:%20s\n", ITEM_key(ptr2), ITEM_data(ptr2)); 
}

int main(int argc, char **argv) {
    settings_init();
    slabs_init(1<<29, 2, true);    
    assoc_init(0); 
    slabs_info();  
    slabs_alloc_test();
    slabs_info();
    sleep(600);
    return 0;
}
