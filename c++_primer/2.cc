#include "head.h"
void bitwidth();
void overflow();
void reference();
int i = 0;

int main(int argc, char *argv[]) {
    
    bitwidth();
    int i = 100;
    std::cout<<"i="<<i<<std::endl;
    //overflow();
    reference();
    return 0;
}

void bitwidth() {
    int i = 0;
    std::cout<<"sizeof(int)="<<sizeof(i)<<std::endl;
    long int j = 0;
    std::cout<<"sizeof(long int)="<<sizeof(j)<<std::endl;
    short int k = 0;
    std::cout<<"sizeof(k)="<<sizeof(k)<<std::endl;

    uint16_t l = 65535;
    uint16_t m = 1;

    uint16_t sum = l + m; //caution: overflow!
    std::cout<<"l="<<l<<std::endl;
    std::cout<<"summ="<<sum<<std::endl;
    
    if(sum<l) {
        std::cout<<"sum<l"<<std::endl;
    }
    else {
        std::cout<<"sum>l"<<std::endl;
    }
    int16_t sum2 = l-1; //caution: overflow too!
    std::cout<<"sum2="<<sum2<<std::endl;
}

void overflow() {
    int32_t i = 100, sum = 0;
    //for(int32_t i = 0; i != 10; ++i) {
    for(; i != 10; ++i) { //it will overflow
        sum += i;
    }
    if(i == 10) {
        std::cout<<"i = 10"<<std::endl;
    }

    std::cout<<"sum = "<<sum<<std::endl;
}

void reference() {
    int i = 10, &j = i;
    ++i;
    j = 5;
    std::cout<<"i = "<<i<<std::endl;
}

