#include "head.h"
/*
 * simple grep tools
 * by charliezhao 
 *
 */
int test_func1();
void usage(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // usage(argc, argv);
    
    if (argc != 3) {
        usage(argc, argv);
    }
    
    char errorinfo[1024] = "";

    const char *pattern = argv[1];
    const char *filename = argv[2];
    
    std::cout<<"pattern = "<<pattern<<std::endl;
    std::cout<<"filename = "<<filename<<std::endl;
    
    struct stat s_st;
    int ret = stat(filename, &s_st);
    if(ret != 0) {
        sprintf(errorinfo, "%s don't exists", filename);
        std::cout<<errorinfo<<std::endl; 
        return -1;
    }

    if(!S_ISREG(s_st.st_mode)) {
        sprintf(errorinfo, "%s not a regular file", filename);
        std::cout<<errorinfo<<std::endl; 
        return -1;
    }

    if(!(s_st.st_mode&S_IRUSR)) {
        perror("user don't have read permission");
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    char *lineptr = NULL;
    size_t len;

    while(getline(&lineptr, &len, fp) != -1) {
        if(strstr(lineptr, pattern) != NULL) {
            std::cout<<lineptr;
        }
    }

    std::cout<<std::endl;
    
    free(lineptr);
    // test_func1(); 


    return 0;
}
void usage(int argc, char *argv[]) {
    std::cout<<"usage:";
    std::cout<<argv[0];
    std::cout<<" [pattern] [filename]"<<std::endl;
   
    exit(-1);
}


int test_func1() {
    const char *filename = "./grep.dat";
    int fd = open(filename, O_APPEND|O_CREAT, S_IRUSR|S_IWUSR);
    if(fd == -1) {
        perror("open file fail");
        return -1;
    }
    
    char buffer[1024];
    int cnt;
    while((cnt = read(fd, buffer, 1024))>0) {
        std::cout<<buffer;
    }
    std::cout<<std::endl;
    return 0;
}

