#include <iostream>
#include <string.h>

using namespace std;

int index(){
    const char *s = "-a-ab-abc-adbc";
    const char *t = "abc";
    size_t len_s = strnlen(s, 1024);
    size_t len_t = strnlen(t, 1024);
    int i=0, j=0;

    while(i<len_s && j <len_t) {
        if(s[i] == t[j]) {
            ++i;
            ++j;
        }
        else {
            i = i-j+1;
            j = 0;
        }
    }
    if(j >= len_t) {
        return (i - len_t);
    }else {
        return 0;
    }
}

int main(int argc, char **argv) {
    std::cout<<"index()="<<index()<<std::endl;
    return 0;
}
