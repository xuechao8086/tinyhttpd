#include "head.h"
char  *find_char_one(char const *source, char const *chars);
const char *find_char_two(char const *str, char const *dst);
char * match(char *str, char *want);
char * _match(char *str, char *want); 
int del_substr(char *str, const char *substr);
void reverse_string(char *string);
void get_prim_num(int limit);
void get_prim_num2(int limit);

int main(int argc, char *argv[]) {

    char const source[] = {"xuechao zhao"};
    char const chars[] = {"zhao"};

    std::cout<<"source = "<<source<<", chars ="<<chars<<std::endl;
    std::cout<<"find_char_two(source, chars) = "<<find_char_two(source, chars)<<std::endl;
    
    char source2[] = {"xuechao zhao zhao"};
    char chars2[] = {"zhao"};
    const char *chars3 = {"zhao"};
    std::cout<<"match(source2, chars2) = "<<match(source2, chars2)<<std::endl;
    
    std::cout<<"del_substr(source2, char3) = "<<del_substr(source2, chars3)<<std::endl;
    std::cout<<"source2 = "<<source2<<std::endl;
    

    char test[] = {"1234567890"};
    reverse_string(test);
    std::cout<<"test = "<<test<<std::endl;
    

    get_prim_num(10);
    get_prim_num2(1024);
    
    int i = 5;
    int array[5*i];

    for(int j = 0; j < i; ++j) {
        std::cout<<array[i] <<" ";
    }
    std::cout<<std::endl;
    return 0;
}

char * find_char(char const *source, char const *chars) {
    int i = 0;
    int j = 0;
    while(*(source+i) && *(chars + j)) {
        if(*(source+i) == *(chars+j)) {
            ++i;
            ++j;
        } else {
            i = i - j + 1; 
            j = 0;
        }
    }

    if(*(chars + j - 1) == '\0') {
        return (char *)source + i;
    }
    else {
        return NULL;
    }
}

const char *find_char_two(char const *str, char const *dst) {
    char const *cp;
    if(str != NULL && dst != NULL) {
        for(; *str != '\0'; str++) {
            for(cp = dst; *cp != '\0'; cp++) {
                if(*str == *cp) {
                    return (char const *)str;
                }
            }
        }
    }
    return NULL;
}

char * _match(char *str, char *want) {
    char *i = str, *j = want;
    while(*j) {
        if(*i++ != *j++) {
            return NULL;
        }
    }
    return str;
}

char * match(char *str, char *want) {
    while(*str) {
        char *ok = _match(str, want);
        if(ok != NULL) {
            return ok;
        } else {
            ++str;
        }
    }
    return NULL;
}

int del_substr(char *str, const char *substr) {
    char *pos = match(str, (char *)substr);
    if(pos == NULL) {
        return 0;
    } 
    else {
        int j = 0;
        for(;*(substr + j); ++j);
        
        for(; *(pos+j); *pos = *(pos + j), ++pos);
        
        *pos = '\0';
        return 1;
    }
}

void reverse_string(char *string) {
    char *s = string;
    char *e = string;
    char t;
    for(;*e; ++e);
    --e;

    for( ;s<e; ++s, --e) {
        t = *s;
        *s = *e;
        *e = t;
    }
}

void get_prim_num(int limit) {
    bool * array = (bool *)malloc((limit+1)*sizeof(bool));
    for(int i = 0; i <= limit; ++i, array[i] = true);
    
    for(int i = 2; i <= limit; ++i) {
        for (int j = i+1; j <= limit; ++j) {
            if (j % i == 0) {
                *(array + j) = false;
            }
        }
    }
    for(int i = 0; i <= limit; ++i) {
        if(*(array+i)) {
            std::cout<<i<<" ";
        }
    }
    std::cout<<std::endl;

    free(array);
}

void get_prim_num2(int limit) {
    int len = limit/2 + limit%2;
    //bool *array = (bool *) malloc(len*sizeof(bool));
    bool array[len]; //it's ok, because limit and len is const type 
    for(int n = 0; n < len; array[n++] = true);

    for(int n = 0; n < len; ++n) {
        int val = 2*n + 1;
        for(int j = 2 ; j<=limit; ++j) {
            if(val <= j) {
                continue;
            }
            if(val%j == 0) {
                array[n] = false;
            }
        }
    }
    
    for(int i = 0; i < len; ++i) {
        if(array[i]) {
            std::cout<<2*i + 1<<" ";
        }
    }

    std::cout<<std::endl;

    //free(array);

}

