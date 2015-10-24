#include "head.h"
/*
 * simple grep tools
 * by charliezhao 
 *
 */
class GrepProcessor {
    public:
        GrepProcessor(const char *ptn, const char **fns, int cnt);
        int processor();
        ~GrepProcessor();

    private:
        int _processor(const char *filename);
        const char *pattern;
        std::vector<const char *> filenames;
};

GrepProcessor::GrepProcessor(const char *ptn, const char **fns, int cnt) {
    pattern = ptn;
    for(int i = 2; i < cnt; ++i) {
        filenames.push_back(*(fns+i));
    }
}

GrepProcessor::~GrepProcessor() {
    std::cout<<__func__<<" called"<<std::endl;
}

int GrepProcessor::processor() {
    std::vector<const char *>::iterator iter;
    for(iter = filenames.begin(); iter != filenames.end(); ++iter) {
        _processor(*iter);
    }
    return 0;
}

int GrepProcessor::_processor(const char *filename) {
    bool mark = false;
    char errorinfo[1024] = "";
    //std::cout<<"$:filename:"<<filename<<", pattern:"<<pattern<<std::endl;
    
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
            mark = true;
        }
    }
    
    if(!mark) {
        std::cout<<"$ "<<filename<<" no match"<<std::endl;
    }

    fclose(fp); 
    free(lineptr);
    return 0;
} 

int grep(const char *filename, const char *pattern);
int test_func1();
void usage(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    // usage(argc, argv);
    
    if (argc < 3) {
        usage(argc, argv);
    }

    /*
    const char *pattern = argv[1]; 
    for(int i = 2; i < argc; ++i) {
        const char *filename = argv[i];
        grep(filename, pattern); 
    }

    // test_func1(); 
    */
    
    const char *pattern = argv[1];
    GrepProcessor p(pattern, (const char **)argv, argc); 
    p.processor();

    return 0;
}
void usage(int argc, char *argv[]) {
    std::cout<<"usage:";
    std::cout<<argv[0];
    std::cout<<" [pattern] [filename]"<<std::endl;
   
    exit(-1);
}

int grep(const char *filename, const char *pattern) {
    char errorinfo[1024] = "";
    std::cout<<"# filename:"<<filename<<", pattern:"<<pattern<<std::endl;
    
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


    return 0;
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

