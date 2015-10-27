#include "head.h"
#define BUFLEN 1024

void usage(const char *name);
class Command {
    public:
        Command(const char *command):command(command) {}
        ~Command() {fclose(fp);}

        void run(void);

    private:
        const int len = 1024;
        const char *command;
        FILE *fp;
        const char * const mode = "r";
};

void Command::run(void) {
    const char *sleep = "sleep 10s;";
    std::string c = sleep + command;
    std::cout<<c<<std::endl;
    fp = popen(c.c_str(), mode);
    char buf[len];
    while(fread(buf, sizeof(char), len, fp)) {
        std::cout<<buf;
    }
    std::cout<<std::endl;
}

int main(int argc, char *argv[]) {
    if(argc == 1) {
        usage(argv[0]);
    }
    for(int i = 1; i < argc; ++i) {
        Command cm(argv[i]);
        cm.run();
    }

    return 0;
}

void usage(const char *name) {
    std::cout<<name<<" [command1[command2[...]]]"<<std::endl;
    exit(0);
}

