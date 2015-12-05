/*************************************************************************
	> File Name:    test.cpp
	> Author:       charliezhao
	> Mail:         xuechao8086@126.com
	> Created Time: 2015-12-03 20:10
 ************************************************************************/
#include "memcache.h"

int item_test() {
    int maxi = 0; 
    
    //test set.
    for(int i = 0; i < 1024; i++) {
        char key[1024];
        memset(key, 0, 1024);
        sprintf(key, "charlie_%d", i);
        const size_t nkey = strlen(key) + 1;
        const int flags = 0;
        const time_t exptime = 0;
        const int nbytes = 1024; 
        uint32_t cur_hv = jenkins_hash((void *)key, nkey);
        item *it = do_item_alloc(key, nkey, flags, exptime, nbytes, cur_hv);
        if(it == NULL) {
            fprintf(stderr, "\033[31malloc fail\033[0m");
            maxi = i;
            break; 
        }
        memcpy(ITEM_data(it), (void *)&i, sizeof(int));
    }
    
    //test get.
    for(int i = 0; i < maxi; ++i) {
        char key[1024];
        memset(key, 0, 1024);
        sprintf(key, "charlie_%d", i); 
        const size_t nkey = strlen(key) + 1;
        uint32_t cur_hv = jenkins_hash((void *)key, nkey);
        item *it = assoc_find(key, nkey, cur_hv);
        if(it == NULL) {
            fprintf(stderr, "\033[31mget fail\033[0m");
            return -1;
        }
        int val = 0;
        memcpy((void *)&val, ITEM_data(it), sizeof(val));
        // fprintf(stdout, "key:%s value:%d\n", ITEM_key(it), val);
        if(i&0x1) {
            item_free(it); 
        }
    }
    return 0;
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

