#include "head.h"
void test_switch(int val);
void _6_12();

int main(int argc, char *argv[]) {

    test_switch(3);
    _6_12();
    return 0;
}

void test_switch(int val) {
    switch(val) {
        case 1:
            // int i = 10; //cann't have i here.
            std::cout<<"val = 1"<<std::endl;
            // std::cout<<"i = "<<i<<std::endl;
            break;
        case 2:
            std::cout<<"val = 2"<<std::endl;
            break;
        case 3:
            //int j = 20; //cann't have j here.
            std::cout<<"val = 3"<<std::endl;
            // std::cout<<"j = "<<j<<std::endl;
            break;
        default:
            std::cout<<"default, val = "<<val<<std::endl;
    }
}
    
void _6_12() {
    std::string preWord, curWord;
    
    std::string maxWord;

    int currcnt = 0, maxcnt = 0;
    while(std::cin>>curWord) {
        std::cout<<"curWord = "<<curWord<<std::endl;
        if(preWord == curWord) {
            ++currcnt;
        }
        else {
            currcnt = 1;
        }

        preWord = curWord;
        if(currcnt > maxcnt) {
            maxWord = curWord;
        }

    }
    std::cout<<maxWord<<std::endl;
}
