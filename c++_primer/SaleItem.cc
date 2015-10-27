#include "SaleItem.h"

int main(int argc, char *argv[]) {
    Sales_item s1, s2;
     
    std::cin>>s1;
    std::cin>>s2;

    Sales_item s3 = s1+s2; 


    std::cout<<"s1"<<std::endl;
    std::cout<<"&s1 = "<<&s1<<std::endl;
    std::cout<<s1<<std::endl;
    
    std::cout<<"s2"<<std::endl;
    std::cout<<"&s2 = "<<&s2<<std::endl;
    std::cout<<s2<<std::endl;

    std::cout<<"s3"<<std::endl;
    std::cout<<"&s3 = "<<&s3<<std::endl;
    std::cout<<s3<<std::endl;
    return 0;
}

