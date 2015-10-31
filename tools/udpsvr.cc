#include "head.h"
int listenfd = 0;
void udpsvr_one();
void udpsvr_two();
void do_sigio(int sig) {
    struct sockaddr_in cli_addr;
    char buf[1024];
    memset(buf, 0, 1024);

    int cli_len = sizeof(cli_addr);
    
    recvfrom(listenfd, buf, 1024, 0, (struct sockaddr *) &cli_addr, (socklen_t *)&cli_len);

    std::cout<<buf;
}

int main(int argc, char *argv[]) {
    udpsvr_one();
    return 0;
}

void udpsvr_one() {
    struct sockaddr_in svr_addr;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(8888);
    svr_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sigaction sigio_action;
    memset(&sigio_action, 0, sizeof(sigio_action));
    sigio_action.sa_flags = 0;
    sigio_action.sa_handler = do_sigio;
    sigaction(SIGIO, &sigio_action, NULL);

    fcntl(listenfd, F_SETOWN, getpid());

    int flags = fcntl(listenfd, F_GETFL, 0);
    flags |= O_ASYNC | O_NONBLOCK;
    fcntl(listenfd, F_SETFL, flags);

    bind(listenfd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
    
    sigset_t set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGIO);
    //pthread_sigmask(SIG_BLOCK, &set, NULL);
    
    while(true) {
        // method one, sleep
        // sleep(1);

        // method two, pause until signal
        // pause();
        //
        //
        
        sigwait(&set, &sig);
    }
    close(listenfd);
}
void udpsvr_two() {
    struct sockaddr_in svr_addr;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(8888);
    svr_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int fd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(fd, (struct sockaddr *)&svr_addr, sizeof(svr_addr));
    
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr)); 
    socklen_t len = sizeof(cli_addr);

    
    while(true) {
        char buf[1024] = "\0";
        recvfrom(fd, buf, 1024, 0, (struct sockaddr *) &cli_addr, &len);
        std::cout<<"########################"<<std::endl;
        std::cout<<buf<<std::endl;
        sleep(2);
    }

    close(fd);
}
