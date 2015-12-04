/************************************************************************* 
	> File Name: hash.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue 17 Nov 2015 06:20:39 PM PST
 ************************************************************************/
#include "memcache.h"
#include "jenkins_hash.h"
#include "murmur3_hash.h"

int hash_init(enum hash_func type) {
    if(type == JENKINS_HASH) {
        hash = jenkins_hash;
        settings.hash_algorithm = "jenkins";
        return 0;
    }
    else if(type == MURMUR3_HASH) {
        hash = MurmurHash3_x86_32;
        settings.hash_algorithm = "murmur3";
        return 0;
    }
    else {
        return -1;
    }
}

