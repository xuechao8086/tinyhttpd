/*
 * batch commands tools.
 * by charliezhao
 *
 * memo:
 *      local fd do not use select, find no use.
 * todo:
 *      network support.
 */
#include "head.h"
#define BUFLEN 1024
#define MAX_EVENTS 1024

const bool debug = true;

void usage(const char *name);

class TcpServer {
    public:
        TcpServer(uint32_t port);
        ~TcpServer();
        
        int run(void); 

    private:
        int domain_;
        int type_;
        int protocol_ = 0;
        uint32_t port_;
         
        int sfd_ = 0;
};

int  TcpServer::run(void) {
    if((sfd_ = socket(domain_, type_, 0)) == -1) {
        perror("create socket fail");
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port=htons(port_);
    
    bool re_use_addr = true;
    setsockopt(sfd_, SOL_SOCKET, SO_REUSEADDR, (const void *)&re_use_addr, sizeof(re_use_addr));

    if(bind(sfd_, (const sockaddr *)&my_addr, sizeof(my_addr)) < 0)  {
        perror("bind fail");
    }

    listen(sfd_, 5);

    //setup epoll
    int epollfd = epoll_create(10);
    if(epollfd == -1) {
        perror("epoll create fail");
        return -1;
    }
    
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = sfd_;
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sfd_, &ev) == -1) {
        perror("epoll_ctl fail");
        return -1;
    }
    
    int nfds = 0;
    int cfd = 0;
    struct sockaddr_in remote_addr;    
    socklen_t addrlen = sizeof(sockaddr_in);
    const char *hello = "hello from charliezhao"; 
    char buf[BUFLEN] = "\0"; 
    
    while(true) {
       nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
       if(nfds == -1) {
           perror("epoll_wait fail");
           return -1;
       }
       for(int n = 0; n < nfds;  ++n) {
           if(events[n].data.fd == sfd_) {
               if((cfd = accept(sfd_, (sockaddr *)&remote_addr, &addrlen)) == -1) {
                   perror("accept fail");
                   continue;
               }
               fcntl(cfd,F_SETFL,O_NONBLOCK);
               ev.events = EPOLLIN | EPOLLET; 
               ev.data.fd = cfd; 
               if(epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &ev) == -1) {
                   perror("epoll_ctl fail");
                   continue;
               }
           }
           else {
               //more work need to do.
               recv(events[n].data.fd, buf, BUFLEN, 0);
               std::cout<<buf;
           }
       }
    }
    
    /*
    int client_sockfd;
    while(true) { 
        if((client_sockfd = accept(sfd_, (sockaddr *)&remote_addr, &addrlen)) == -1) {
            perror("accept fail");

        }

        std::cout<<"accept from "<<inet_ntoa(remote_addr.sin_addr)<<std::endl;

        char buf[BUFLEN] = "hello from charliezhao\n";
        send(client_sockfd, buf, sizeof(buf), 0);

        memset(buf, '\0', sizeof(buf));
        recv(client_sockfd, buf, BUFLEN, 0);
        std::cout<<buf<<std::endl; 
        close(client_sockfd);
    }
    */
}

TcpServer::TcpServer(uint32_t port) {
    port_ = port;

    domain_ = AF_INET;
    type_ = SOCK_STREAM;
    
}

TcpServer::~TcpServer() {
    close(sfd_);
}

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

    if(debug) { 
        for(auto fd : fds) {
            std::cout<<"fd:"<<fd<<", set:"<<FD_ISSET(fd, &rfds)<<std::endl;
        }

        std::cout<<"fd:"<<10<<", set:"<<FD_ISSET(10, &rfds)<<std::endl;
    }

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
    TcpServer s(9999);
    s.run();


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

