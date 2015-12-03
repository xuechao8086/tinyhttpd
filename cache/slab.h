/*************************************************************************
	> File Name: slab.h
	> Author: charliezhao 
	> Mail: 
	> Created Time: Wed 18 Nov 2015 02:32:58 AM PST
 ************************************************************************/

#ifndef _SLAB_H
#define _SLAB_H


#define POWER_SMALLEST 1
#define POWER_LARGEST  256
#define MAX_NUMBER_OF_SLAB_CLASSES 64

#define CHUNK_ALIGN_BYTES 8

#define ITEM_LINKED 1
#define ITEM_CAS 2

/* temp, unused */
#define ITEM_SLABBED 4
/* Item was fetched at least once in its lifetime */
#define ITEM_FETCHED 8
/* Appended on fetch, removed on LRU shuffling */
#define ITEM_ACTIVE 16

unsigned int slabs_clsid(const size_t size);

void slabs_init(const size_t limit, const double factor, const bool prealloc);
void *slabs_alloc(const size_t size, unsigned int id, unsigned int *total_chunks);
void slabs_free(void *ptr, size_t size, unsigned int id);


typedef struct {
        uint32_t size;      /* sizes of items */
        uint32_t perslab;   /* how many items per slab */

        void *slots;           /* list of item ptrs */
        unsigned int sl_curr;   /* total free items in list */

        unsigned int slabs;     /* how many slabs were allocated for this class */

        void **slab_list;       /* array of slab pointers */
        unsigned int list_size; /* size of prev array */

        unsigned int killing;  /* index+1 of dying slab, or zero if none */
        size_t requested; /* The number of requested bytes */
} slabclass_t;

void slabs_alloc_test(void);
#endif
