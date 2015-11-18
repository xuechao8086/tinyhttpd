/*************************************************************************
	> File Name: hash.h
	> Author: charliezhao
	> Mail: 
	> Created Time: Tue 17 Nov 2015 06:09:37 PM PST
 ************************************************************************/

#ifndef _HASH_H
#define _HASH_H
typedef uint32_t (*hash_func)(const void *key, size_t length);
hash_func hash;

enum hashfunc_type {
    JENKINS_HASH = 0,
    MURMUR3_HASH
};

int hash_init(enum hashfunc_type type);
#endif
