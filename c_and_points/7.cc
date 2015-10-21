#include "head.h"

int hermite(int n, int x);
int gcd(int m, int n);
int ascii_to_integer(char *string);
int max_list(int *arr, int len);
void written_amount(unsigned int amount, char *buffer);

int main(int argc, char *argv[]) {
    std::cout<<"hermite(10) = "<<hermite(10, 1)<<std::endl;
    
    std::cout<<"gcd(1024, 10) = "<<gcd(1024, 10)<<std::endl;
    
    int list[10] = {1, 2, 3, 4, 9999, 5, 6, 7, 8, 9};
    std::cout<<"max(list) ="<<max_list(list, 10)<<std::endl;

    char buffer[1024] = {""};
    
    written_amount(123456, buffer);
    std::cout<<"123456 = "<<buffer<<std::endl;

    while(true) {
        std::cout<<"input num:";
        gets(buffer);
        std::cout<<"buffer = "<<buffer<<", num = "<<ascii_to_integer(buffer)<<std::endl;
    }
    return 0;
}

int hermite(int n, int x) {
    if(n < 1) {
        return 1;
    }
    else if(n == 1) {
        return 2*x;
    }
    else {
        return 2*hermite(n-1, x) - 2*(n-1)*hermite(n-2, x);
    }
}

int gcd(int m, int n) {
    //assume m >= n
    if(m < n) {
        int t = m;
        m = n;
        n = t;
    }

    if(m%n == 0) {
        return n;
    }
    else {
        return gcd(n, m%n);
    }
}

int ascii_to_integer(char *string) {
    int num = 0;
    for(char *point = string; *point; ++point) {
        num = num*10 + (int)(*point);
    }

    return num;
}

int max_list(int *arr, int len) {
    int max = 0;
    for(int i = 0; i < len; ++i) {
        if(*(arr+i) > max) {
            max = *(arr+i);
        }
    }

    return max;
}

static const char *digits[] = {
    " ", "ONE ", "TWO ", "THREE ", "FOUR ", "FIVE ", "SIX ", "SEVEN ",
    "EIGHT ", "NINE ", "TEN ", "ELEVEN ", "TWELVE ", "THIRTEEN ",
    "FOURTEEN ", "FIFTEEN ", "SIXTEEN ", "SEVENTEEN ", "EIGHTEEN ",
    "NINETEEN "
};

static const char    *tens[] = {
    " ", "TEN", "TWENTY ", "THIRTY ", "FORTY ", "FIFTY ", "SIXTY ", "SEVENTY ",
    "EIGHTY ", "NINETY "
};

static const char    *magnitudes[] = {
    "", "THOUSAND ", "MILLION ", "BILLION "
};

void _written_amount(unsigned int amount, char *buffer, int deep){
    // assume 0<amount<100,000,000, just 987, 654, 012
    unsigned int value = amount / 1000;
    if(value > 0) {
        _written_amount(value,  buffer, deep+1);
    }
    
    amount %= 1000;
    int i = amount/100;
    if(i != 0) { 
        strcat(buffer, digits[i]);
        strcat(buffer, "HUNDRED ");
    }
    int j = amount/10%10;
    if(j != 0) {
        strcat(buffer, tens[j]);
    }
    int k = amount%10;
    if (k != 0) {
        strcat(buffer, digits[k]);
    }
    if (deep != 0) {
        strcat(buffer, magnitudes[deep]);
    }
}

void written_amount(unsigned int amount, char *buffer) {
    return _written_amount(amount, buffer, 0); 
}



