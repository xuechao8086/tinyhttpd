#include "head.h"
#define BITARRAYLENGTH 32

uint32_t reverse_bits_32(uint32_t val);
uint64_t reverse_bits_64(uint64_t val);
unsigned int reverse_bits(unsigned int val);
bool set_bit(int bit_array[], unsigned bit_num);

int main(int argc, char *argv[]) {
    unsigned int i =2; 
    unsigned int j = (i>>1);
    std::cout<<"j = "<<j<<std::endl;
    unsigned int k = (j>>1);
    std::cout<<"k = "<<k<<std::endl;

    std::cout<<"reverse_bits_32(25) = "<<reverse_bits_32(25)<<std::endl;
    std::cout<<"reverse_bits_64(25) = "<<reverse_bits_64(25)<<std::endl;
    std::cout<<"reverse_bits(25) = "<<reverse_bits(25)<<std::endl;
    
    int bit_array[] = {0, 0, 0, 0};
    for(int i = 0; i<4; ++i) {
        std::cout<<"bit_array["<<i<<"]="<<bit_array[i]<<" ";
    }
    std::cout<<std::endl;
    set_bit(bit_array, 5);    
    set_bit(bit_array, 12); 
    for(int i = 0; i<4; ++i) {
        std::cout<<"bit_array["<<i<<"]="<<bit_array[i]<<" ";
    }
    std::cout<<std::endl;

    return 0;
}

uint32_t reverse_bits_32(uint32_t val) {
    uint32_t answer = 0;
    for(uint32_t i = 1; i != 0; i<<=1) {
        answer <<= 1;
        if(val & 1) {
            answer |= 1;
        }
        val >>= 1;
    }
    return answer;
}

uint64_t reverse_bits_64(uint64_t val) {
    uint64_t answer = 0;
    for(uint64_t i = 1; i != 0; i<<=1) {
        answer <<= 1;
        if(val & 1) {
            answer |= 1;
        }
        val >>= 1;
    }
    return answer;
}

unsigned int reverse_bits(unsigned int value) {
    unsigned int bitlen = 0; 
    for(unsigned int i = 1; i != 0; i<<=1, bitlen +=1);
    std::cout<<"bitlen = "<<bitlen<<std::endl;
    
    value >>=bitlen;
    return value;
}

bool set_bit(int bit_array[], unsigned bit_num) {
    if(bit_num >= BITARRAYLENGTH) {
        return false;
    }
    
    unsigned pos_one = bit_num/8;
    unsigned pos_two = bit_num%8;
    bit_array[pos_one] |= 1<<pos_two;

    return true;
}



