/*************************************************************************
> File Name: daemon.cpp
> Author: 
> Mail: 
> Created Time: Tue 17 Nov 2015 12:16:42 AM PST
************************************************************************/
#include "daemon.h"

int daemonize(int nochdir, int noclose) {
    pid_t pid = fork();
    if(pid < 0) {
        return -1;
    }
    else if(pid > 0) {
        _exit(EXIT_SUCCESS); 
    }

    if(setsid() == -1) {
        return -1;
    }
    if(nochdir == 0) {
        if(chdir("/") != 0) {
            perror("chdir");
            return -1;
        }
    } 
    if(noclose == 0) {
        int fd = 0;
        if((fd = open("/dev/null", O_RDWR, 0)) != -1) { 
            int stdfd[3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO};
            for(int i = 0; i < 3; ++i) {
                if(dup2(fd, stdfd[i]) < 0) {
                    perror("dup2");
                    return -1;
                }
            }

            if(fd > stdfd[3]) {
                if(close(fd) < 0) {
                    perror("close");
                    return -1;
                }
            } 
        }
        else {
            return -1;
        }

    } 
    return 0;
}

int main(int argc, char **argv) {
    daemonize(0, 0);
    while(true) {
        sleep(1);
    }
    return 0;
}
