#include "head.h"
/*
 * simple grep tools
 * by charliezhao 
 *
 */
class GrepProcessor {
    public:
        GrepProcessor(const char *ptn, const char **fns, int cnt);
        ~GrepProcessor();
        int processor();
        int multi_processor();
        int processor(const char *filename, const char *pattern); //for thread safe version
        void echo();    

    private:
        int _processor(const char *filename);
        void * _processor_thread(void *filename);

    private:
        const char *pattern;
        int pattern_len;

        std::vector<const char *>filenames;
};

//void * test_thread(GrepProcessor *cls, const char *name);
void * test_thread(void *args);
struct s_args {
    GrepProcessor *cls;
    const char *filename;
    const char *pattern;
};
 
GrepProcessor::GrepProcessor(const char *ptn, const char **fns, int cnt) {
    pattern = ptn;
    pattern_len = strlen(pattern);

    for(int i = 2; i < cnt; ++i) {
        filenames.push_back(*(fns+i));
    }
}

int GrepProcessor::processor(const char *filename, const char *pattern) {
    //std::cout<<"$:filename:"<<filename<<", pattern:"<<pattern<<std::endl;
    
    struct stat s_st;
    int ret = stat(filename, &s_st);
    if(ret != 0) {
        return -1;
    }

    if(!S_ISREG(s_st.st_mode)) {
        return -1;
    }

    if(!(s_st.st_mode&S_IRUSR)) {
        return -1;
    }

    FILE *fp = fopen(filename, "r");
    char *lineptr = (char *)malloc(sizeof(char)*10240);
    
    memset(lineptr, '\0', 10240);

    size_t len;

    while(getline(&lineptr, &len, fp) != -1) {
        if(strstr(lineptr, pattern) != NULL) {
            std::cout<<"\033[32m\033[1m";
            std::cout<<filename;
            std::cout<<" \033[31m\033[1m";
            std::cout<<lineptr;
            std::cout<<"\033[0m";
        }
     }

    fclose(fp); 
    free(lineptr);
    return 0;
}

int GrepProcessor::multi_processor() {
    std::vector<pthread_t> ids; 
    
    std::vector<const char *>::iterator iter;
    for(iter = filenames.begin(); iter != filenames.end(); ++iter) { 
        pthread_t id;
        // in c++
        // cann't use class's public method as thread route func!
        // http://blog.sina.com.cn/s/blog_61798d5d01018kz7.html
        //int ret = pthread_create(&id, NULL, this->_processor_thread, &name);
       
        s_args args = {this, *iter, pattern}; 

        int ret = pthread_create(&id, NULL, test_thread, (void *)&args); 
        if (ret != 0) {
            perror("create thread fail");
            return ret;
        }
        ids.push_back(id); 
    }

    std::vector<pthread_t>::iterator iter2;
    for(iter2 = ids.begin(); iter2 != ids.end(); ++iter2) {
        pthread_join(*iter2, NULL);
    }

    return 0;
}

void GrepProcessor::echo() {
    std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" called"<<std::endl;
}

//void * test_thread(GrepProcessor *cls, const char *name) {
void * test_thread(void *args) {
    // (reinterpret_cast<GrepProcessor *>(args))->echo() ;
    // ((GrepProcessor *)args)->echo();
    
    // GrepProcessor *p = (GrepProcessor *)args;
    // GrepProcessor *p = reinterpret_cast<GrepProcessor *>args; 
    
    //cls->processor(name);
    
    //s_args *s = reinterpret_cast<GrepProcessor *> args; //cann't use reinterpret_cast here!

    s_args *s = (s_args *) args;
    GrepProcessor *cls = s->cls;
    const char *filename = s->filename;
    const char *pattern = s->pattern;
    cls->processor(filename, pattern);
    
    return NULL;
}

GrepProcessor::~GrepProcessor() {
    //std::cout<<__func__<<" called"<<std::endl;
}

int GrepProcessor::processor() {
    std::vector<const char *>::iterator iter;
    for(iter = filenames.begin(); iter != filenames.end(); ++iter) {
        _processor(*iter);
    }
    return 0;
}
    
void * GrepProcessor::_processor_thread(void *filename) {
    std::cout<<"name = "<<(const char *)filename<<std::endl;
    return NULL; 
    //return _processor((const char *) filename);
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
            std::cout<<"\033[32m\033[1m";
            std::cout<<filename;
            std::cout<<" \033[31m\033[1m";
            std::cout<<lineptr;
            std::cout<<"\033[0m";
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
    //p.processor();
    p.multi_processor();
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

