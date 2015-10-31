/*
 * batch commands tools.
 * by charliezhao
 * todo:
 *  timeout
 */
#include "commands.h"

int  TcpServer::epollrun(void) {
    int ret;
    if((sfd_ = socket(domain_, type_, 0)) == -1) {
        perror("create socket fail");
    }
    int on = 0;
    // no nagle
    ret = setsockopt(sfd_, IPPROTO_TCP, TCP_NODELAY, (const void *)&on, sizeof(on));
    if( ret == -1) 
    {
        perror("setsockopt TCP_NODELAY fail");
    }
    
    // keep alive 
    bool keepalive = true;
    ret = setsockopt(sfd_, SOL_SOCKET, SO_KEEPALIVE, (const void *)&keepalive, sizeof(keepalive));
    if(ret == -1) {
        perror("setsockopt SO_KEEPALIVE fail");
    }
    
    // set tos
    int tostype = 0xcc;
    if((ret = setsockopt(sfd_, IPPROTO_IP, IP_TOS, (const void *)&tostype, sizeof(tostype))) == -1) {
        perror("setsockopt tos type fail");
    }
    
    // set ttl, has no effect, why?
    int ttl = 0x20;
    if((ret = setsockopt(sfd_, IPPROTO_TCP, IP_TTL, (const void *)&ttl, sizeof(ttl))) == -1) {
        perror("setsockopt ttl fail");
    }
    
    // set recv timeout
    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    if((ret = setsockopt(sfd_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv))) == -1) {
        perror("setsockopt recv timeout fail");
    }


    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = INADDR_ANY;
    my_addr.sin_port=htons(port_);
    
    // set no block. 
    int flags = fcntl(sfd_, F_GETFL, 0);
    if(flags == -1) {
        perror("fcntl get fail");
    }
    ret = fcntl(sfd_, F_SETFL, flags|O_NONBLOCK);
    if(ret == -1) {
        perror("fcntl set fail");
    }


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
    const char *hello = "hello from charliezhao's commands tools\n"; 
    size_t hello_len = strlen(hello)+1;
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
                

                char *ip = inet_ntoa(remote_addr.sin_addr);
                std::cout<<"connected from "<<ip<<std::endl;

                if((ret = send(cfd, hello, hello_len, MSG_DONTWAIT)) == -1) {
                    perror("send fail");
                    continue;
                }


                fcntl(cfd,F_SETFL,O_NONBLOCK);

                // ev.events = EPOLLIN | EPOLLET ; 
                ev.events = EPOLLIN; 
                ev.data.fd = cfd; 
                if(epoll_ctl(epollfd, EPOLL_CTL_ADD, cfd, &ev) == -1) {
                    perror("epoll_ctl fail");
                    continue;
                }
            }
            
            //in test, never appear EPOLLHUP stat, why?
            else if(events[n].events & EPOLLHUP) {
                std::cout<<"EPOLLHUP"<<std::endl;
                std::cout<<events[n].data.fd<<" closed";
                ev.events = EPOLLHUP;
                ev.data.fd = events[n].data.fd;
                if(epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1) {
                    perror("epoll_ctl fail");
                }
                close(events[n].data.fd);
            }

            else if(events[n].events & EPOLLIN) {
                std::cout<<"EPOLLIN"<<std::endl;
                memset(buf, 0, BUFLEN);
                ssize_t len = recv(events[n].data.fd, buf, BUFLEN, 0);
                if (len <= 0) {
                    ev.events = EPOLLHUP;
                    ev.data.fd = events[n].data.fd;
                    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1) {
                        perror("epoll_ctl fail");
                    }
                    close(events[n].data.fd);
                }
                else {
                    int32_t len = 0;
                    int32_t magicbegin = 0;
                    memcpy((void *)&magicbegin, buf, sizeof(int32_t));
                    std::cout<<"magicbegin = 0x";
                    std::cout<<std::setbase(16);
                    std::cout<<magicbegin<<std::endl;
                    
                    len += sizeof(int32_t);

                    struct s_args args;
                    memset((void *)&args, 0, sizeof(args));
                    memcpy((void *)&args,  buf+len, sizeof(args));
                    len += sizeof(args);
                    
                    std::cout<<"p.i = 0x";
                    std::cout<<args.i<<std::endl;
                    std::cout<<"p.j = 0x";
                    std::cout<<args.j<<std::endl;
                    std::cout<<"p.k = 0x";
                    std::cout<<args.k<<std::endl;
                    
                    char msg[1024]="\0"; 
                    strncat(msg, buf+len, 1024);
                    std::cout<<"msg = "<<msg<<std::endl;
                    len += strlen(msg) + 1;
                    
                    int32_t magicend = 0;
                    memcpy((void *)&magicend, buf+len, sizeof(int32_t));
                    std::cout<<"magicend = 0x";
                    std::cout<<magicend<<std::endl;

                    continue;

                    // for test struct
                    struct s_args *p = (struct s_args *)buf;
                    std::cout<<"p->i = "<<p->i<<std::endl;
                    std::cout<<"p->j = "<<p->j<<std::endl;
                    std::cout<<"p->k = "<<p->k<<std::endl;
                    std::cout<<"p->m = "<<p->m<<std::endl;
                    continue;

                    std::cout<<buf;
                    ev.events = EPOLLOUT | EPOLLHUP;
                    ev.data.fd = cfd;
                    if(epoll_ctl(epollfd, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1) {
                        perror("epoll_ctl fail");
                        continue; 
                    }
                }
            }
            else if(events[n].events & EPOLLOUT) {
                std::cout<<"EPOLLOUT"<<std::endl;
                
                // for test
                // ev.events = EPOLLIN | EPOLLHUP;
                // ev.data.fd = events[n].data.fd;
                // if(epoll_ctl(epollfd, EPOLL_CTL_MOD, ev.data.fd, &ev) == -1) {
                //    perror("epoll_ctl fail");
                // }
                // continue;
                // end of test

                char buf[0];
                int ret = send(events[n].data.fd, buf, 0, 0);
                if(ret != 0) {
                    std::cout<<"send fail"<<std::endl;
                    ev.events = EPOLLHUP;
                    ev.data.fd = events[n].data.fd;
                    if(epoll_ctl(epollfd, EPOLL_CTL_DEL, events[n].data.fd, &ev) == -1) {
                        perror("epoll_ctl fail");
                    }
                    close(events[n].data.fd);                    
                }
                else {
                    ev.events = EPOLLIN | EPOLLHUP;
                    ev.data.fd = events[n].data.fd;
                    if(epoll_ctl(epollfd, EPOLL_CTL_MOD, events[n].data.fd, &ev) == -1) {
                        perror("epoll_ctl fail");
                    }
                }

            }
            else if(events[n].events & EPOLLPRI) {
                memset(buf, 0, BUFLEN);
                recv(events[n].data.fd, buf, BUFLEN, 0);
                std::cout<<buf;
            }

            else {
                //more work need to do.
                std::cout<<"more work need to do"<<std::endl;  
            }
        }
    }
}

TcpServer::TcpServer(uint32_t port) {
    port_ = port;

    domain_ = AF_INET;
    type_ = SOCK_STREAM;

}

TcpServer::~TcpServer() {
    close(sfd_);
}


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

AsyncIO::AsyncIO(int argc, char **argv) {
    for(int i = 1; i < argc; ++i) {
        filenames_.push_back(*(argv+i));
    }
}

AsyncIO::~AsyncIO() {
}

void AsyncIO::test(void) {
}


int main(int argc, char *argv[]) {
    if(argc == 1) {
        aiousage(argv[0]);
    }

    return aiotest(argc, argv); 
    
    TcpServer s(9999);
    s.epollrun();


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

void aiousage(const char *name) {
    std::cout<<name<<" [filename1[filename2[...]]]"<<std::endl;
    exit(0);
}

int aiotest(int argc, char **argv) {
    int numReqs = argc -1; // total request num
    
    struct ioRequest *ioList = (struct ioRequest *)calloc(numReqs, sizeof(ioRequest));
    if(ioList == NULL) {
        perror("ioList NULL;");
        return -1;
    }

    struct aiocb *aiocbList = (struct aiocb *)calloc(numReqs, sizeof(aiocb));
    if(aiocbList == NULL) {
        perror("aiocbList NULL;");
        return -1;
    }

    struct sigaction sa, osa_quit, osa_aio;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = quitHandler;
    if(sigaction(SIGQUIT, &sa, &osa_quit) == -1) {
        perror("sigaction fail");
        return -1;
    }
   
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_handler = aioSigHandler;
    if(sigaction(IO_SIGNAL, &sa, &osa_aio) == -1) {
        perror("sigaction fail");
        return -1;
    }

    for(int j = 0; j < numReqs; ++j) {
        ioList[j].reqNum = j;
        ioList[j].status = EINPROGRESS;
        ioList[j].aiocbp = &aiocbList[j];
        ioList[j].aiocbp->aio_fildes = open(argv[j+1], O_RDONLY);
        ioList[j].aiocbp->aio_buf = malloc(BUF_SIZE);
        ioList[j].aiocbp->aio_nbytes = BUF_SIZE;
        ioList[j].aiocbp->aio_reqprio = 0;
        ioList[j].aiocbp->aio_offset = 0;
        ioList[j].aiocbp->aio_sigevent.sigev_notify = SIGEV_SIGNAL;
        ioList[j].aiocbp->aio_sigevent.sigev_signo = IO_SIGNAL;
        ioList[j].aiocbp->aio_sigevent.sigev_value.sival_ptr = &ioList[j];

        if(ioList[j].aiocbp->aio_fildes == -1) {
            perror("open fail");
            return -1;
        }
        if(ioList[j].aiocbp->aio_buf == NULL) {
            std::cout<<"malloc fail"<<std::endl;
            return -1;
        }
        std::cout<<"open "<<argv[j+1]<<" on descriptor "<<ioList[j].aiocbp->aio_fildes<<std::endl;
        
        int s = aio_read(ioList[j].aiocbp);
        if(s == -1) {
            std::cout<<"aio_read fail"<<std::endl;
            return -1;
        }
    }
        
    for(int openReqs = numReqs; openReqs > 0; ) {
        sleep(3);
        if(gotSIGQUIT) {
            std::cout<<"got SIGQUIT; canceling I/O requests"<<std::endl;
            for( int j = 0; j < numReqs; ++j ) {
                if(ioList[j].status == EINPROGRESS) {
                    std::cout<<"request "<<j<<" on descriptor "<<ioList[j].aiocbp->aio_fildes<<std::endl;

                    int s = aio_cancel(ioList[j].aiocbp->aio_fildes, ioList[j].aiocbp);

                    if(s == AIO_CANCELED) {
                        std::cout<<"I/O canceled"<<std::endl;
                    }
                    else if(s == AIO_NOTCANCELED) {
                        std::cout<<"I/O not calceled"<<std::endl;
                    }
                    else if(s == AIO_ALLDONE) {
                        std::cout<<"I/O all done"<<std::endl;
                    }
                    else {
                        perror("aio_cancel fail");
                        return -1;
                    }
                }
            }
            gotSIGQUIT = 0;
        }
        
        std::cout<<"aio_error()"<<std::endl;
        for(int j = 0; j < numReqs; ++j) {
            if(ioList[j].status == EINPROGRESS) {
                std::cout<<"for request "<<j<<" descriptor "<<ioList[j].aiocbp->aio_fildes<<" ";
                ioList[j].status = aio_error(ioList[j].aiocbp);
                switch(ioList[j].status) {
                    case 0:
                        std::cout<<"I/O successed"<<std::endl;
                        break;
                    case EINPROGRESS:
                        std::cout<<"in progress"<<std::endl;
                        break;
                    case ECANCELED:
                        std::cout<<"cancled"<<std::endl;
                        break;
                    default:
                        perror("aio_error");
                        break;
                }

                if(ioList[j].status != EINPROGRESS) {
                    --openReqs;
                }
            }
        }
    }

    std::cout<<"All I/O requests complete"<<std::endl;

    std::cout<<"aio_return():"<<std::endl;
    for(int j = 0; j < numReqs; ++j) {
        ssize_t s = aio_return(ioList[j].aiocbp);

        std::cout<<"for request "<<j<<" descriptor "<<ioList[j].aiocbp->aio_fildes;
        std::cout<<" len "<<s<<std::endl;

    }
    
    if(sigaction(SIGQUIT, &osa_quit, NULL) == -1) {
        perror("sigaction fail");
        return -1;
    }
 
    return 0;
}
