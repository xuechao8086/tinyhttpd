/*
 * batch commands tools.
 * by charliezhao
 * todo:
 *  add epoll support
 *  add select support
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
        int run(void); // use multi thread.
        int epoll_run(void); // use epoll.
        int select_run(void); // use select.
        void test(void);

    private:
        int _run(void);
        std::stack<const char *> cmds;
        std::mutex mutex_;
};

int BatchCommand::epoll_run(void) {
     
    return 0;
}

int BatchCommand::select_run(void) {
    fd_set rfds;
    FD_ZERO(&rfds);
    int nfds = 0; 
    std::vector<int> fds;
    while(!cmds.empty()) {
        const char *cmd = cmds.top();
        FILE *fp = popen(cmd, "r");
        cmds.pop(); 
        FD_SET(fp->_fileno, &rfds);
        fds.push_back(fp->_fileno);
        ++nfds;
    }
    
    for(auto fd : fds) {
        std::cout<<"fd:"<<fd<<", set:"<<FD_ISSET(fd, &rfds)<<std::endl;
    }
     
    std::cout<<"fd:"<<10<<", set:"<<FD_ISSET(10, &rfds)<<std::endl;
    
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10;

    // select have no effect, why?
    int retval = select(nfds, &rfds, NULL, NULL, &timeout);
    
    if(retval == -1) {
        perror("select fail");
        return retval;
    }

    std::cout<<"select retval "<<retval<<std::endl;

    char buf[1025] = "\0";
    int ret = 0; 
    for(auto fd : fds) {
        //if(FD_ISSET(fd, &rfds)) {
        if(true) {
            int offset = 0;
            while(true) {
                ret = read(fd, buf, 1024);
                if(ret == 0) {
                    break;
                }
                if(ret == -1) {
                    break;
                }
                buf[ret] = '\0';
                std::cout<<buf;
                offset += 1024;
                lseek(fd, offset, SEEK_SET);
            }
        }
    }
    return 0;
}

void BatchCommand::test(void) {
    FILE *fp = fopen("./pregnancytime.cc", "r");
    int fd = fp->_fileno;
    //int fd = open("./pregnancytime.cc", 0, O_RDONLY); 
    int ret = 0;
    char buf[1024] = "\0";
    int offset = 0;
    while(true) {
        ret = read(fd, buf, 1024);
        if(ret == 0) {
            break;
        }
        else if(ret == -1) {
            break; 
        }
        offset += 1024;
        lseek(fd, offset, SEEK_SET);
        buf[ret] = '\0'; 
        std::cout<<buf;
    }
    fclose(fp);
    std::cout<<std::endl;
}


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
    bc.select_run(); 
    //bc.test();
    //bc.run();
     
    return 0;
}

void usage(const char *name) {
    std::cout<<name<<" [command1[command2[...]]]"<<std::endl;
    exit(0);
}

