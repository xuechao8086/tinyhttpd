#include "head.h"
#define ODDNUM(x) (x)&0x1

void teststring();
void testarr();
int get_size();
void testvector();

int main(int argc, char *argv[]) {
    teststring();
    testarr();
    std::cout<<"12/3*4+5*15+24%4/2 = "<<12/3*4+5*15+24%4/2<<std::endl;
    for(int i = 0; i < 10; ++i) {
        int j = random()%356;
        if(ODDNUM(j)) {
            std::cout<<j<<"is odd"<<std::endl;
        }
        else {
            std::cout<<j<<"is even"<<std::endl;
        }
    }

    uint64_t ul1 = 3;
    uint64_t ul2 = 7;
    std::cout<<"ul1&ul2 = "<<(ul1&ul2)<<std::endl;
    std::cout<<"ul1|ul2 = "<<(ul1|ul2)<<std::endl;
    std::cout<<"ul1&&ul2 = "<<(ul1&&ul2)<<std::endl;
    std::cout<<"ul1||ul2 = "<<(ul1||ul2)<<std::endl;

    testvector();
    return 0;
}

void testvector() {
    const char *filename = "./makefile";
    std::cout<<"content of"<<filename<<std::endl;
    FILE *fp = fopen(filename, "r");
    char *lineptr = NULL;
    size_t len = 0;
    
    std::vector<std::string *> v;

    while(getline(&lineptr, &len, fp) != -1) {
        std::string *s = new std::string(lineptr);
        v.push_back(s); 
    }
    free(lineptr);

    for(uint32_t i = 0; i < v.size(); ++i) {
        std::cout<<*v[i];
    }
    std::cout<<"again:"<<std::endl;
    for(auto it = v.begin();  it != v.end(); ++it) { //c++ 11 stand
        std::cout<<**it<<std::endl;;
    }
    
    std::cout<<"again2:"<<std::endl;
    for(auto &x: v) { //c++ 11 stand
        std::cout<<*x<<std::endl;
    }


}

void teststring() {
    std::string s;
    std::cout<<s[1]<<std::endl;
    std::vector<std::string> svec(10, "null");
    for(std::vector<std::string>::size_type ix = 0; ix < svec.size(); ++ix) {
        std::cout<<"svec["<<ix<<"] = "<< svec[ix] <<std::endl;
    }


    std::string s2 = "12345";
    size_t idx = 0;
    int64_t result = std::stoll(s2, &idx, 10);
    std::cout<<"result = "<<result<<std::endl;
}

void testarr() {
    unsigned buffer_size = 1024;
    int ia[buffer_size];
    memset((void *)ia, 0, buffer_size*sizeof(int));
    for(int i = 1000; i < 1024; ++i) {
        std::cout<<"ia["<<i<<"]="<<ia[i]<<std::endl;
    }
    for(int i = 1; i < 10; ++i) {
        std::cout<<"ia["<<i<<"]="<<ia[i]<<std::endl;
    }
    
    int *ia2 = (int *)malloc(1024*sizeof(int));
    memset((void *)ia2, 0, 1024*sizeof(int)); //0 ok!
    memset(ia2, 0, 1024*sizeof(int)); //1 not ok, think about it, op byte!
    for(int i = 1000; i < 1024; ++i) {
        std::cout<<"ia2["<<i<<"]="<<ia2[i]<<std::endl;
    }

    for(int i = 1; i < 10; ++i) {
        std::cout<<"ia2["<<i<<"]="<<ia2[i]<<std::endl;
    }
    free(ia2);
    
    //int rnd = get_size();
    int rnd = 1024;
    const char buffer[1024]= {"charliezhao"};
    std::cout<<"buffer = "<<buffer<<std::endl; 

}

int get_size() {
    int64_t rnd = random()%356;
    if (rnd > 100) {
        return rnd;
    }
    else {
        return 1024;
    }

}

