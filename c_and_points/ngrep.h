#include "head.h"

class GrepProcessImpl {
    public:
        GrepProcessImpl();
        ~GrepProcessImpl();
        int process(const char *filename, const char *pattern);
};

class GrepProcess {
    public:
        GrepProcess(const char *format, const char **filenames, int cnt);
        ~GrepProcess();

        int run();

    private:
        const char *pattern;
        std::stack<const char *> m_st;
        GrepProcessImpl *gpi;
};
