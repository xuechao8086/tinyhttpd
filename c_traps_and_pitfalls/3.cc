#include "head.h"
int test_one(int cnt, int args2);
int test_two();
int test_three();
int main(int argc, char *argv[]) {
    test_one(10, 10);
    test_two();
    test_three();
    int a = 10;
    int b = 20;
    int c = a+++++b;
    std::cout<<"a+++++b++="<<c<<std::endl;
    return 0;
}
    
int test_one(int cnt, int args2) {
    if(cnt) {
        std::cout<<"cnt = "<<cnt<<std::endl;
        printf("&cnt = %p\n", &cnt);
        printf("&args2 = %p\n", &args2);
        int  arr[10], i = 0;
        std::cout<<"&i = "<<&i<<std::endl;
        std::cout<<"sizeof(i) = "<<sizeof(i)<<std::endl; 
        for(; i <= 10; ++i) {
            arr[i] = 0;
        }
        
        
        for(int j = 0; j <= 10; ++j) {
            std::cout<<"&arr["<<j<<"] = "<<&arr[j]<<std::endl;
        }
        const char *name = "xuechao";
        std::cout<<"name = "<<&(*name)<<std::endl;
        uint64_t k = 1024;
        printf("&k = %p\n", &k);
        char buffer[1024] = "xuechao";
        char buffer2[1024] = "name"; 
        printf("buffer = %p\n", buffer);
        printf("buffer2 = %p\n", buffer2);
        std::cout<<"#################################"<<std::endl;
        return test_one(cnt-1, args2+1); 
    } else  {
        return 0;
    }
}

int test_two() {
    int a;  
    int b;  
    int c;  
    int p[5];      
    printf("a  %p\n",&a);  
    printf("b  %p\n",&b);  
    printf("c  %p\n",&c);  
    printf("p0    %p\n",&p[0]);  
    printf("p1    %p\n",&p[1]);  
    printf("p2    %p\n",&p[2]);  
    printf("p3    %p\n",&p[3]);  
    printf("p4    %p\n",&p[4]); 
    return 0;
}

int test_three(){
    std::cout<<"#################################"<<std::endl;
    volatile int arr[10];
    volatile int j = 10;
    volatile int k = 12;

    volatile int i;
    for(i = 0; i <= 10; ++i) {
        arr[i] = 0;
        printf("&arr[%d] = %p\n",i,  &arr[i]);
    }
    printf("&i = %p\n", &i);
    printf("&j = %p\n", &j);
    printf("&k = %p\n", &k);
    return 0;
}

