#include "util.h"

bool safe_strtoull(const char *str, uint64_t *out) {
    assert(out != NULL);
    errno = 0;
    *out = 0;
    char *endptr;
    unsigned long long ull = strtoull(str, &endptr, 10);
    if ((errno == ERANGE) || (str == endptr)) {
        return false;
    }

    if (xisspace(*endptr) || (*endptr == '\0' && endptr != str)) {
        if ((long long) ull < 0) {
            /* only check for negative signs in the uncommon case when
             * the unsigned number is so big that it's negative as a
             * signed number. */
            if (strchr(str, '-') != NULL) {
                return false;
            }
        }
        *out = ull;
        return true;
    }
    return false;
}

bool safe_strtoll(const char *str, int64_t *out) {
    assert(out != NULL);
    errno = 0;
    *out = 0;
    char *endptr;
    long long ll = strtoll(str, &endptr, 10);
    if ((errno == ERANGE) || (str == endptr)) {
        return false;
    }

    if (xisspace(*endptr) || (*endptr == '\0' && endptr != str)) {
        *out = ll;
        return true;
    }
    return false;
}

bool safe_strtoul(const char *str, uint32_t *out) {
    char *endptr = NULL;
    unsigned long l = 0;
    assert(out);
    assert(str);
    *out = 0;
    errno = 0;

    l = strtoul(str, &endptr, 10);
    if ((errno == ERANGE) || (str == endptr)) {
        return false;
    }

    if (xisspace(*endptr) || (*endptr == '\0' && endptr != str)) {
        if ((long) l < 0) {
            /* only check for negative signs in the uncommon case when
             * the unsigned number is so big that it's negative as a
             * signed number. */
            if (strchr(str, '-') != NULL) {
                return false;
            }
        }
        *out = l;
        return true;
    }

    return false;
}

bool safe_strtol(const char *str, int32_t *out) {
    assert(out != NULL);
    errno = 0;
    *out = 0;
    char *endptr;
    long l = strtol(str, &endptr, 10);
    if ((errno == ERANGE) || (str == endptr)) {
        return false;
    }

    if (xisspace(*endptr) || (*endptr == '\0' && endptr != str)) {
        *out = l;
        return true;
    }
    return false;
}

void vperror(const char *fmt, ...) {
    int old_errno = errno;
    char buf[1024];
    va_list ap;

    va_start(ap, fmt);
    if (vsnprintf(buf, sizeof(buf), fmt, ap) == -1) {
        buf[sizeof(buf) - 1] = '\0';
    }
    va_end(ap);

    errno = old_errno;

    perror(buf);
}

#ifndef HAVE_HTONLL
static uint64_t mc_swap64(uint64_t in) {
#ifdef ENDIAN_LITTLE
    /* Little endian, flip the bytes around until someone makes a faster/better
    * way to do this. */
    int64_t rv = 0;
    int i = 0;
     for(i = 0; i<8; i++) {
        rv = (rv << 8) | (in & 0xff);
        in >>= 8;
     }
    return rv;
#else
    /* big-endian machines don't need byte swapping */
    return in;
#endif
}

uint64_t ntohll(uint64_t val) {
   return mc_swap64(val);
}

uint64_t htonll(uint64_t val) {
   return mc_swap64(val);
}
#endif

int err(const char *msg){
    perror(msg);
    exit(0);
}

int strip_blank(char *buf, int len) {
    if(len == 1) {
        buf[0] = '\0'; 
        return 0;
    } 

    int end = len - 1; 
    char *p = buf + end;
    while((*p == ' ') && (end >= 0)) {
        --end;
        --p;
    }
    if(end == 0) {
        buf[0] = '\0';
        return 0;
    }
    
    int beg = 0;
    p = buf;
    
    while((*p == ' ') && (beg < end)) {
        ++beg;
        ++p;
    }
    if((end != len - 1) || (beg != 0)) {
        int i = 0;
        for(int j = beg; j <= end; ++j) {
            buf[i++] = buf[j];
        }
        buf[i] = '\0';
    } 
    return 0;
}
#ifdef _UTIL_TEST 
int main(int argc, char **argv) { 
    const char *str = "19860903";
    uint64_t val1 = 0;
    int64_t val2 = 0;
    uint32_t val3 = 0;
    int32_t val4 = 0;

    safe_strtoull(str, &val1) || err("safe_strtoull");
    safe_strtoll(str, &val2) || err("safe_strtoll");
    safe_strtoul(str, &val3) || err("safe_strtoul");
    safe_strtol(str, &val4) || err("safe_strtol");

    std::cout<<"val1 = "<<val1<<std::endl;
    std::cout<<"val2 = "<<val2<<std::endl;
    std::cout<<"val3 = "<<val3<<std::endl;
    std::cout<<"val4 = "<<val4<<std::endl;

    return 0;
}
#endif
