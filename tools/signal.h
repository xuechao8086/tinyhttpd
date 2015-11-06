#ifndef SIGNAL_H
#define SIGNAL_H

#include "head.h"
namespace util {
    void test(void);

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
    
    // for pthread
    const int NTHR = 10;
    const int64_t NUMNUM = 100000000;
    const int TNUM = NUMNUM/NTHR;
    
    int64_t nums[NUMNUM];
    int64_t snums[NUMNUM];
    
    int complong(const void *args1, const void *args2);
    void *thr_fn(void *arg);
    void merge(); 
    void merge2();
    int thread_test(void);

    // thread barrier
    pthread_barrier_t b;
    
    // thread cond 
    struct msg {
        struct msg *m_next;
    };
    struct msg *workq;    
    pthread_cond_t qready = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
    
    void * process_msg(void *);
    //void enqueue_msg(struct msg *mp);
    void * enqueue_msg(void *mp);
    int test_msg(void);
}

#endif
