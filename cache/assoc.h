/*************************************************************************
	> File Name: assoc.h
	> Author: charliezhao 
	> Mail: 
	> Created Time: Wed 02 Dec 2015 07:21:38 PM PST
 ************************************************************************/

#ifndef ASSOC_H
#define ASSOC_H

#define HASHPOWER_DEFAULT 16
#define hashsize(n) ((uint64_t)1<<n)
#define hashmask(n) (hashsize(n)-1)


void assoc_init(const int hashpower_init);
item *assoc_find(const char *key, const size_t nkey, const uint32_t hv);
int assoc_insert(item *item, const uint32_t hv);
void assoc_delete(const char *key, const size_t nkey, const uint32_t hv);

extern unsigned int hashpower;
extern unsigned int item_lock_hashpower;

#endif
