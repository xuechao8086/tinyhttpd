/*
 * add multi thread support
 * by charliezhao
 */
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

class BatchCommand {
    public:
        BatchCommand(int cnt, char **commands)
        {
            for(int i = 1; i < cnt; ++i) {
                cmds.push(*(commands+i));
            }
        }
        
        ~BatchCommand() {}
        int run(void);
    
    private:
        int _run(void);
        std::stack<const char *> cmds;
        std::mutex mutex_;
};

int  BatchCommand::run(void) {
    int32_t len = cmds.size();
    int8_t thread_cnt;

    if(len<10) {
        thread_cnt = 1;
    } 
    else if(len<100) {
        thread_cnt = 2;
    }
    else if(len<1000) {
        thread_cnt = 3;
    }
    else {
        thread_cnt = 4;
    }
    
    std::thread *arr[thread_cnt];
    for(int8_t i = 0; i <= thread_cnt; ++i) {
        arr[i] = new std::thread(std::bind(&BatchCommand::_run, this));
    }
    
    for(int8_t i = 0; i <= thread_cnt; ++i) {
        arr[i]->join();
        delete arr[i];
    }
    
    return 0;
}

int BatchCommand::_run(void) {
    while(true) {
        mutex_.lock();
        if(!cmds.empty()) {
            const char *cmd = cmds.top();
            cmds.pop();
            mutex_.unlock();
            Command c(cmd);
            c.run();
        }
        else {
            mutex_.unlock();
            break;
        }
    }
    return 0;
}

void Command::run(void) {
    const char *sleep = "sleep 10s; ";
    std::string c(sleep);
    c += command;
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
    /*
    for(int i = 1; i < argc; ++i) {
        Command cm(argv[i]);
        cm.run();
    }
    */

    BatchCommand bc(argc, argv);
    bc.run(); 
    return 0;
}

void usage(const char *name) {
    std::cout<<name<<" [command1[command2[...]]]"<<std::endl;
    exit(0);
}

