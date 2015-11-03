/*
 * memo: you can send signal from another process or another thread
 *
 */

#include "head.h"

namespace util {
    // for signal SIGCHLD
    void sig_chld(int signo) { 
        int status = 0;
        pid_t pid = wait(&status);
        std::cout<<"child "<<pid<<" terminated"<<std::endl;
    }
    
    // for signal SIGPIPE
    void sig_int(int signo);
    void sig_pipe(int signo);
    void sig_quit(int signo);
    
    void sig_int2(int signo, siginfo_t *si, void *ucontext);
    int test_sigpending(void);
    int test_sigaction(void);
    int send_sig_quit(const char *, int32_t pid);

    // pid info
    int write_pid(const char *path);
    int read_pid(const char *path);

    // system v shm
    const int ARRAY_SIZE = 4000;
    const int MALLOC_SIZE = 100000;
    const int SHM_SIZE = 100000;
    const int SHM_MODE = 0666;
    const int SHMID = 0xee; 

    int shm_test(void);
    int shm_test2(void);
    int shm_test3(void);

    // posix shm
    int posix_shm_test(void);
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

void util::sig_int2(int signo, siginfo_t *si, void *ucontext) {
    std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__;
    std::cout<<" called"<<std::endl;
    
    std::cout<<"si_signo = "<<si->si_signo<<std::endl;
    std::cout<<"si_pid = "<<si->si_pid<<std::endl;
    std::cout<<"si_uid = "<<si->si_uid<<std::endl;
    std::cout<<"si_fd = "<<si->si_fd<<std::endl;
    std::cout<<"si_value.sival_ptr = "<<si->si_value.sival_ptr<<std::endl;
    std::cout<<"(char *) (si->si_value.sival_ptr) = "<<(char *)(si->si_value.sival_ptr)<<std::endl;
    std::cout<<"(char *) si->si_value.sival_ptr = "<<(char *)si->si_value.sival_ptr<<std::endl;
}

int util::test_sigaction(void) {
    struct sigaction act, oldact;
    memset(&act, 0, sizeof(act));
    memset(&oldact, 0, sizeof(oldact));
    
    act.sa_sigaction = util::sig_int2;
    act.sa_flags = SA_SIGINFO;
    
    if(sigaction(SIGQUIT, &act, &oldact) == -1) {
        std::cout<<"sigaction fail"<<std::endl;
        return -1; 
    }
    
    std::thread child_thread(util::send_sig_quit, "SIGQUIT from child",(int32_t) getpid());
    pause();
    child_thread.join();

    std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__;
    std::cout<<" finish"<<std::endl;
    return 0;
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
    
    
    std::cout<<"SIGQUIT unblocked"<<std::endl;
    std::cout<<"SIGINT unblocked"<<std::endl;

    sleep(5);

    return 0;

}

int util::send_sig_quit(const char *msg, int32_t pid) {
    sleep(60);
    union sigval value;
    memset(&value, 0, sizeof(value));
    
    // value.sival_ptr = malloc(1024*sizeof(char));
    // memset(value.sival_ptr, 0, 1024);
    // const char *src = "hello from signal by charliezhao";
    // memcpy(value.sival_ptr, (const void *)src, strlen(src)+1); 
    
    value.sival_ptr = (void *)msg;

    if(sigqueue(pid, SIGQUIT, value) == -1) {
        perror("sigqueue fail");
        return -1; 
    }

    return 0;
}

int util::write_pid(const char *path) {
    FILE *fp = fopen(path, "w+");
    if(fp == NULL) {
        perror("fopen fail");
        return -1;
    }

    char buf[10] = "\0";
    sprintf(buf, "%d", (int32_t) getpid());
    fwrite((const void *)buf, 10, 1, fp);
    fclose(fp);
    return 0;
}

int util::read_pid(const char *path) {
    int fd = open(path,  O_RDONLY);
    if(fd == -1) {
        perror("open fail");
        return -1;
    }
    char buf[10] = "\0";
    read(fd, buf, 10);
    close(fd);
    
    int32_t pid = atoi(buf);
    return pid;
}

int util::shm_test(void) {
    char array[util::ARRAY_SIZE];
    std::cout<<"array[] from "<<(void *)&array[0];
    std::cout<<" to "<<(void *)&array[util::ARRAY_SIZE];
    std::cout<<std::endl;

    int shmid = 0;
    if((shmid == shmget(IPC_PRIVATE, util::SHM_SIZE, util::SHM_MODE)) < 0) {
        perror("shmget fail");
        return -1;
    }
    
    char *shmptr = NULL;
    if((shmptr = (char *)shmat(shmid, NULL, 0)) == (void *) -1) {
        perror("shmat error");
        return -1;
    }

    if(shmctl(shmid, IPC_RMID, 0) < 0) {
        perror("shmctl error");
        return -1;
    }
    
    sleep(600);
    return 0;
}

int util::shm_test2(void) {
    int id = shmget(util::SHMID, 1024, util::SHM_MODE|IPC_CREAT);
    if(id == -1)  {
        perror("shmget fail");
        return -1;
    }

    void *ptr = (char *)shmat(id, NULL, 0);
    if(ptr == (void *) -1) {
        perror("shmat fail");
        return -1;
    }
    
    const char buf[1024] = "hello from charliezhao for shm\n";
    memcpy(ptr, (const void *)buf, 1024);

    std::cout<<"(char *)ptr = "<<(char *)ptr;
    std::cout<<std::endl;
    
    return 0;
}

int util::shm_test3(void) {
    void *ptr = shmat(98306, NULL, 0);
    std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" ";
    std::cout<<(char *)ptr<<std::endl;
    return 0;
}

int util::posix_shm_test(void) {
    const char *name = "posix_shm.dat";
    int fd = shm_open(name, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if(fd == -1) {
        perror("shm_open fail");
        return -1;
    }
    
    const char *buf = "hello from charliezhao@sharememory.posix\n";
    write(fd, buf, strlen(buf)+1);
    close(fd);
    
    if(shm_unlink(name) == -1) {
        perror("shm_unlink fail");
        return -1;
    }
    return 0;
}


int main(int argc, char *argv[]) {
    util::posix_shm_test();
    util::shm_test2(); 
    // util::shm_test();
    int pipefd[2] = {0, 0};
    
    if(pipe(pipefd) < 0) {
        perror("pipe fail");
        return -1;
    }

    pid_t pid = fork();
    if(pid < 0) {
        perror("fork fail");
        return -1;
    }
    else if(pid == 0) { 
        util::shm_test3();
        close(pipefd[0]);
        
        pid_t rpid = getpid();
        write(pipefd[1], (const void *) &rpid, sizeof(rpid));

        close(pipefd[1]);

         
        //util::write_pid("/home/charlie/tmp/child.pid"); 
        
        int ret = util::test_sigaction();     
        std::cout<<"child process, pid:"<<getpid();
        std::cout<<" finish"<<std::endl;
        return ret;
    }
    else {
        close(pipefd[1]);
        int cpid = 0;
        read(pipefd[0], (void *)&cpid, sizeof(cpid));
        std::cout<<"child pid(from father):"<<cpid<<std::endl;
        close(pipefd[0]); 

        //use pipe instead to get pid
        // util::write_pid("/home/charlie/tmp/father.pid"); 
        // sleep(1);
                 
        //int32_t cpid = util::read_pid("/home/charlie/tmp/child.pid");
        util::send_sig_quit("SIGQUIT from father", cpid);

        int status = 0;
        if(wait(&status) == -1) {
            perror("wait fail");
            return -1;
        }

        std::cout<<"father process, pid:"<<getpid();
        std::cout<<" finish"<<std::endl;
        return 0;
    }

}
