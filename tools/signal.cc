#include "head.h"

namespace util {
    // for signal SIGCHLD
    void sig_chld(int signo) { 
        int status = 0;
        pid_t pid = wait(&status);
        std::cout<<"child "<<pid<<" terminated"<<std::endl;
    }
    // for signal SIGPIPE
    void sig_pipe(int signo);
    int test_sigpending(void);
    void sig_quit(int signo);
    void sig_int(int signo);
}
void util::sig_quit(int signo) {
    std::cout<<"catch SIGQUIT"<<std::endl;
    if(signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
        perror("cann't reset SIGQUIT");
    }
}

void util::sig_int(int signo) {
    std::cout<<"catch SIGINT"<<std::endl;
    if(signal(SIGINT, SIG_DFL) == SIG_ERR) {
        perror("cann't reset SIGINT");
    }
}

int util::test_sigpending(void) {
    // SIGQUIT ctrl + '\'
    // SIGINT ctrl + c
    
    sigset_t newmask, oldmask, pendmask;
    if(signal(SIGQUIT, util::sig_quit) == SIG_ERR) {
        perror("signal SIGQUIT fail");
        return -1;
    }
    
    if(signal(SIGINT, SIG_DFL) == SIG_ERR) {
       perror("signal SIGINT fail");
       return -1;
    }

    sigemptyset(&newmask);
    if(sigaddset(&newmask, SIGQUIT) == -1) {
        perror("sigaddset fail");
        return -1;
    }
    // if(sigaddset(&newmask, SIGINT) == -1) {
    //    perror("sigaddset fail");
    //    return -1;
    // }
    
    // if(sigsuspend(&newmask) == -1) {
    //    perror("sigsuspend fail");
    //    return -1;
    // }
    std::cout<<"sigsuspend finish"<<std::endl;

    if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) == -1) {
        perror("sigprocmask fail");
        return -1;
    }

    //sleep(30);
    struct timespec sp;
    sp.tv_sec = 30;
    sp.tv_nsec = 1;
    nanosleep(&sp, NULL);




    if(sigpending(&pendmask) < 0) {
        perror("sigpending fail");
        return -1; 
    }
    
    if(sigismember(&pendmask, SIGQUIT)) {
        std::cout<<"SIGQUIT pending"<<std::endl;
    }
    if(sigismember(&pendmask, SIGINT)) {
        std::cout<<"SIGINT pending"<<std::endl;
    }

    if(sigprocmask(SIG_SETMASK, &oldmask, NULL)< 0) {
        perror("sigpending fail");
        return -1;
    }
    
    std::cout<<"SIGQUIT unblocked"<<std::endl;
    std::cout<<"SIGINT unblocked"<<std::endl;

    sleep(5);

    return 0;

}

int main(int argc, char *argv[]) {
    return util::test_sigpending();
}
