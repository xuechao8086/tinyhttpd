#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include<iostream>

#define MAXLENGTH 128 
#define MAXLINE 4 

void delblank(char string[]);
char * convert(char str[]);

int main(int argc, char *argv[])
{ 
    char buffer[MAXLENGTH];
    char *input[MAXLINE];
    char *output[MAXLINE]; 
    
    std::cout << "max line length = " << MAXLENGTH <<std::endl;

//why wrong?
//    while(1) {
//        std::cout<<"i = "<<i<<std::endl;
//        if(i>=MAXLINE) {
//            break;
//        }
//
//        gets(buffer);
//        int bufferlen = strlen(buffer);
//        assert(bufferlen<MAXLENGTH);
//        
//        
//        input[i] = (char *)malloc((bufferlen+1)*sizeof(char));
//        if(input[i] == NULL) {
//            std::cout<<"input["<<i<<"] = NULL"<<std::endl;
//            return -1;
//        }
//        strncpy(input[i], buffer, MAXLENGTH);
//        
//        std::cout<<"input[" <<i<<"] = "<<input[i]<<std::endl;
//        ++i;
//    }
    
//    char str[] = "xuechaozhao    tingting zhang";
//    std::cout<<"origin string = "<<str<<std::endl;
//    delblank(str);
//    std::cout<<"string = "<<str<<std::endl;

    for(int cnt = 0; cnt < MAXLINE; ++cnt) {
        input[cnt] = (char *)malloc(MAXLENGTH*sizeof(char));
        gets(buffer);
        delblank(buffer);
        strncpy(input[cnt], buffer, MAXLENGTH);
        *(input[cnt] + strlen(buffer)) = '\0';
    }

    for(int j = 0; j < MAXLINE; ++j) {
        std::cout<<"input["<<j<<"] = "<<input[j]<<std::endl;
    }
    
    int k = 0;
    for(int i = 0; i < MAXLINE; ++i) {
        for (int j = i+1; j < MAXLINE; ++j) {
            if (strcmp(input[i], input[j]) == 0) {
                bool mark = false;
                for(int l = 0; l < k; ++l) {
                    if (strcmp(input[i], output[l]) == 0) {
                        mark = true;
                        break;
                    }
                }
                if(mark == false){
                    output[k] = (char *)malloc(MAXLENGTH*sizeof(char));
                    strncpy(output[k], input[i], MAXLENGTH);
                    *(output[k]+strlen(input[i])+1) = '\0';
                    ++k;
                }
            }
        }
    }
    
    for(int i = 0; i<k; ++i) {
        std::cout<<"output["<<i<<"] = "<<output[i]<<std::endl;
    }


    return 0;
}

void delblank(char str[]) {
    /*latter consider how to finish the function*/
    char *origin = str;

    for(char *i = str; *i != '\0'; ++i) { 
        if(*i != ' ')
        { 
            continue;  
        }
        else {
            int j = 0;
            while(*(i+j) == ' ') {
                ++j;
            }
            --j;
            if (j>1) {
                for(char *k=i, *l=i+j; ; ++k, ++l) {
                    *k = *l;
                    if (*l == '\0') {
                        *k = '\0';
                        break;
                    }
                }
            }
        }
    }
}

char * convert(char *str) {
    for(char *i = str; *i != '\0'; ++i) {
        if(*i >= 'A' && i <= 'Z')  {
            *i += 'a' - 'A';
        }
    }
    return str;
}
            
    
