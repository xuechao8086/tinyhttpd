#include "ngrep.h"

GrepProcessImpl::GrepProcessImpl(){ }

GrepProcessImpl::~GrepProcessImpl() {}

int GrepProcessImpl::process(const char *filename, const char *pattern) {
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

    char *lineptr = (char *)malloc(sizeof(char)*10240);
    memset(lineptr, '\0', 10240);
    size_t len = 0;

    FILE *fp = fopen(filename, "r");
    try {
        while(getline(&lineptr, &len, fp) != -1) {
            *(lineptr+10240-1) = '\0';
            if(strstr(lineptr, pattern) != NULL) {
                std::cout<<"\033[32m\033[1m";
                std::cout<<filename;
                std::cout<<" \033[31m\033[1m";
                std::cout<<lineptr;
                std::cout<<"\033[0m";
            }
         }
    }
    catch(...) {
        std::cout<<"catch error"<<std::endl;
    }

    fclose(fp); 
    free(lineptr);
    return 0;
}

GrepProcess::GrepProcess(const char *format, const char **filenames, int cnt) {
    for(int i = 2; i < cnt; ++i) {
        m_st.push(*(filenames+i));
    }
    pattern = format;
    gpi = new GrepProcessImpl();
}

GrepProcess::~GrepProcess() {
    delete gpi;
}

int GrepProcess::run() {
    while(!m_st.empty()) {
        gpi->process(m_st.top(), pattern);
        m_st.pop(); 
    }

    return 0;
}
int main(int argc, const char *argv[]) {

    const char *format = argv[1];
    const char **filenames = argv;
    int cnt = argc;

    GrepProcess inst(format, filenames, cnt);
    inst.run();

    return 0;
}
