#ifndef JENKINS_HASH_H
#define    JENKINS_HASH_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <iostream>

uint32_t jenkins_hash(const void *key, size_t length);

#endif    /* JENKINS_HASH_H */

