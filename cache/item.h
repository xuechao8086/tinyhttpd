/*************************************************************************
	> File Name: item.h
	> Author: charliezhao 
	> Mail: 
	> Created Time: Tue 17 Nov 2015 08:27:01 PM PST
 ************************************************************************/

#ifndef _ITEM_H
#define _ITEM_H

typedef struct _stritem {
    struct _stritem *next;
    struct _stritem *prev;
    struct _stritem *h_next;
    time_t time;
    time_t exptime; /* how long this can be */
    size_t nbytes;
    uint8_t refcount;
    uint8_t nsuffix;
    uint8_t it_flags;
    uint8_t slabs_clsid;
    uint8_t nkey;

    union {
        uint64_t cas;
        char end;
    } data[];
} item;

#define ITEM_get_cas(i) (((i)->it_flags & ITEM_CAS) ? \
        (i)->data->cas : (uint64_t)0)

#define ITEM_set_cas(i,v) { \
    if ((i)->it_flags & ITEM_CAS) { \
        (i)->data->cas = v; \
    } \
}

#define ITEM_key(item) (((char*)&((item)->data)) \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))

#define ITEM_suffix(item) ((char*) &((item)->data) + (item)->nkey + 1 \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))

#define ITEM_data(item) ((char*) &((item)->data) + (item)->nkey + 1 \
         + (item)->nsuffix \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))

#define ITEM_ntotal(item) (sizeof(struct _stritem) + (item)->nkey + 1 \
         + (item)->nsuffix + (item)->nbytes \
         + (((item)->it_flags & ITEM_CAS) ? sizeof(uint64_t) : 0))

#define LARGEST_ID 256

#define HOT_LRU 0
#define WARM_LRU 64
#define COLD_LRU 128
#define NOEXP_LRU 192

uint64_t get_cas_id(void);

// key     the key
// nkey    the length of key, include \0
// flags   key flags 
// nbytes  number of bytes to hold value,
// suffix  buffer for flags nbytes
// return total size of the header

item *do_item_alloc(const char *key, const size_t nkey, 
                    const int flags, 
                    const time_t exptime, 
                    const int nbytes, 
                    const uint32_t cur_hv);
int do_item_link(item *it, const uint32_t hv);
void do_item_unlink(item *it, const uint32_t hv);
void do_item_remove(item *it);

void item_free(item *it);
void item_link(item *it);
void item_unlink(item *it);

void *lru_traverse(void *args);

#endif
