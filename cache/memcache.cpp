/*************************************************************************
	> File Name: main.cpp
	> Author: charliezhao 
	> Mail: 
	> Created Time: Thu 03 Dec 2015 05:59:19 PM PST
 ************************************************************************/

#include "memcache.h"

int main(int argc, char **argv) {
    settings_init();
    slabs_init(1<<29, 2, true);    
    assoc_init(0); 
    slabs_info();  
    //slabs_alloc_test();
    item_test(); 
    slabs_info();
    item_test(); 
    slabs_info();
    sleep(600);
    return 0;
}
