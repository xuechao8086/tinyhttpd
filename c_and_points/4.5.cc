#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include<iostream>

#define MAXLENGTH 128 
#define MAXLINE 4 

void delbalnk(char string[]);

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

    for(int cnt = 0; cnt < MAXLINE; ++cnt) {
        input[cnt] = (char *)malloc(MAXLENGTH*sizeof(char));
        gets(buffer);
        strncpy(input[cnt], buffer, MAXLENGTH);
        *(input[cnt] + strlen(buffer)) = '\0';
    }

    for(int j = 0; j < MAXLINE; ++j) {
        std::cout<<"input["<<j<<"] = "<<input[j]<<std::endl;
    }
    
    for(int i = 0; i < MAXLINE; ++i) {
        for (int j = i+1; j < MAXLINE; ++j) {
            if (strcmp(input[i], input[j]) == 0) {
                std::cout<<"result = "<<input[i]<<std::endl;
            }
        }
    }
    
    return 0;
}

void delblank(char string[]) {
    /*latter consider how to finish the function*/
    char *origin = string;

    for(char *i = string; *i != '\0'; ++i) {
        if(*i != ' '){
           continue;  
        }
        else {
            int j = 0;
            while(*(i+j) == ' ') {
            }
            
        }
    }
}