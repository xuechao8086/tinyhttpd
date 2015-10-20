#include <stdio.h>
#include <stdlib.h>

#include <iostream>

int match_output(int *mark, int pos, char left, char right);

int main(int argc, char *argv[]) {
    std::cout<<"this program check brackets match perfect"<<std::endl;

    int mark[] = {0, 0, 0};
    enum type {PARENTHESIS=0, BRACE=1}; 
    char val;

    while((val = getchar()) != '\n') {
        switch(val) {
            case '{':
                mark[BRACE] += 1;
                break;
            case '}':
                mark[BRACE] -= 1;
                break;
            case '(':
                mark[PARENTHESIS] += 1;
                break;
            case ')':
                mark[PARENTHESIS] -= 1;
                break;
            default:
                continue;
        }

    }
    
    match_output(mark, 0, '(', ')');
    match_output(mark, 1, '{', '}');

    return 0;
}

int match_output(int *mark, int pos, char left, char right) {
    int val = *(mark+pos);

    if(val > 0) {
        std::cout<<"'"<<left<<"' > '"<<right<<"'"<<std::endl;
    }
    else if (val == 0) {
        std::cout<<"'"<<left<<"' = '"<<right<<"'"<<std::endl;
    }
    else {
        std::cout<<"'"<<left<<"' < '"<<right<<"'"<<std::endl;
    }

    return 0;
}


