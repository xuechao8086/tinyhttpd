#include "signal.h"

void * util::process_msg(void *args) {
    using namespace util;
    struct msg *mp;

    while(true) {
        pthread_mutex_lock(&qlock);
        while(workq == NULL) {
            pthread_cond_wait(&qready, &qlock);
        }
        mp = workq;
        
        std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" ";
        std::cout<<std::endl;
        workq = mp->m_next;
        pthread_mutex_unlock(&qlock);
    }
    return NULL;
}

//void * util::enqueue_msg(struct msg *mp) {
void * util::enqueue_msg(void *args) {
    using namespace util;
    struct msg *mp = (struct msg *)args;
    while(true) {
        pthread_mutex_lock(&qlock); 
        mp->m_next = workq;
        workq = mp;
        std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" ";
        std::cout<<std::endl;
        pthread_mutex_unlock(&qlock);
        //pthread_cond_signal(&qready);
        pthread_cond_broadcast(&qready);
        sleep(1);
    }
    return args;
}


int util::test_msg(void) {
    using namespace std; 
    struct msg *mp = (struct msg *)calloc(1, sizeof(struct msg));
    mp->m_next = NULL;
    pthread_t tid[2];
    pthread_create(&tid[0], NULL, process_msg, (void *) NULL);
    pthread_create(&tid[1], NULL, enqueue_msg, (void *)mp); 
    for(int8_t i = 0; i < 2; ++i) { 
        pthread_join(tid[i], NULL);
    }
    return 0;
}

int util::test_msg2(void) {
    using namespace std;
    struct msg *mp = (struct msg *)calloc(1, sizeof(struct msg));
    mp->m_next = NULL;
    pthread_t id[2];
    pthread_attr_t attr[2];
    
    typedef void* (*start_routine)(void *);
    
    start_routine func[2] = {process_msg, enqueue_msg}; 
    
    for(int8_t i = 0;  i < 2; ++i) {
        if(pthread_attr_init(&attr[i]) != 0) {
            perror("pthread_attr_init fail");
            return -1;
        }
        if(pthread_attr_setdetachstate(&attr[i], PTHREAD_CREATE_DETACHED) != 0) {
            perror("pthread_attr_setdetachstate fail");
            return -1;
        }

    }
    
    pthread_create(&id[0], &attr[0], func[0], (void *)NULL);
    pthread_create(&id[1], &attr[1], func[1], (void *)mp);
    
    pause();
    pthread_attr_destroy(&attr[0]);
    pthread_attr_destroy(&attr[1]);

    return 0;
}

int util::test_msg3(int argc, char **argv) {
    pthread_t thr;
    pthread_attr_t attr;

    if(argc > 1) {
        pthread_attr_t *attrp = &attr;

        if(pthread_attr_init(&attr) != 0)  {
            perror("pthread_init fail");
            return -1;
        }

        if(pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
            perror("pthread_attr_setdetachstate fail");
            return -1;
        }
        
        if(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
            perror("pthread_attr_setinheritsched fail");
            return -1;
        }

        uint64_t stack_size = strtoul(argv[1], NULL, 10);

        void *sp = NULL;
        if(posix_memalign(&sp, sysconf(_SC_PAGESIZE), stack_size) !=  0) {
            perror("posix_memalign");
            return -1;
        }

        std::cout<<"posix_memalign() allocated at "<<sp<<std::endl;

        if(pthread_attr_setstack(&attr, sp, stack_size) != 0) {
            perror("pthread_attr_setstacksize");
            return -1; 
        }
        
        if(pthread_create(&thr, attrp, &util::thread_property, NULL) != 0) {
            perror("pthread_create fail");
            return -1;
        }
        
        if(pthread_attr_destroy(attrp) != 0) {
            perror("pthread_attr_destroy fail");
            return -1;
        }
    }
    return 0;
}

void * util::thread_property(void *args) {
    pthread_attr_t gattr;
    if(pthread_getattr_np(pthread_self(), &gattr) != 0) {
        perror("pthread_getattr_np fail");
        return NULL;
    }
    
    int detachstate = 0;
    if(pthread_attr_getdetachstate(&gattr, &detachstate) != 0) {
        perror("pthread_attr_getdetachstate fail");
        return NULL;
    }
    
    int scope = 0;
    if(pthread_attr_getscope(&gattr, &scope) != 0) {
        perror("pthread_attr_getscope");
        return NULL;
    }
    
    int inheritsched = 0;
    if(pthread_attr_getinheritsched(&gattr, &inheritsched) != 0) {
        perror("pthread_attr_getinheritsched");
        return NULL;
    }

    std::cout<<"detachstate = "<<detachstate<<std::endl;
    
    return NULL;
}


void util::test(void) {
    int32_t arr[16];
    memset(arr, 1986, sizeof(arr));
    for(int32_t i = 0; i < 16; arr[i] = i*10, ++i);
    for(int32_t i = 0; i < 16; ++i) {
        std::cout<<"arr["<<i<<"] = "<<arr[i]<<std::endl;
    }
}

int util::complong(const void *args1,  const void *args2) {
    int64_t i = *(int64_t *)args1;
    int64_t j = *(int64_t *)args2;

    if(i > j) {
        return -1;
    }
    else if(i == j) {
        return 0;
    }
    else {
        return 1;
    }
}

void *util::thr_fn(void *arg) {
    int64_t idx = (int64_t)arg;

    qsort(&util::nums[idx], util::TNUM, sizeof(int64_t), util::complong);

    pthread_barrier_wait(&util::b);
    return ((void *)0);
}

void util::merge(void) {
    int64_t idx[util::NTHR];
    
    for(int64_t i = 0; i < util::NTHR; idx[i] = i*util::TNUM, i++);
    for(int64_t sidx = 0; sidx < util::NUMNUM; ++sidx) {
        int64_t num = LONG_MAX;
        int64_t minidx = 0;
        for(int64_t i = 0; i < util::NTHR; ++i) {
            if((idx[i] < (i+1)*util::TNUM) && (util::nums[idx[i]] < num)) {
                num = util::nums[idx[i]];
                minidx = i; 
            }
        }
        util::snums[sidx] = util::nums[idx[minidx]];
        ++idx[minidx];
    }
}

void util::merge2(void) {
    // to be done
    for(int64_t i = 0; i < util::NUMNUM; ++i) {
        for(int j = 0; j < util::NTHR; ++j) {
        }
    }

}

int util::thread_test(void) {
    pthread_t tid[util::NTHR]; 

    srandom(1);
    for(int64_t i = 0; i < util::NUMNUM; ++i) {
        util::nums[i] = random();
    }
    
    struct timeval start;
    gettimeofday(&start, NULL);
    pthread_barrier_init(&util::b, NULL, util::NTHR+1);
    for(int64_t i = 0; i < util::NTHR; ++i) {
        if(pthread_create(&tid[i], NULL, util::thr_fn, (void *)(i *util::TNUM)) == -1) {
            perror("pthread_create fail");
            return -1;
        }
    }

    pthread_barrier_wait(&util::b);
    util::merge();

    struct timeval end; 
    gettimeofday(&end, NULL);

    int64_t startusec = start.tv_sec*1000000 + start.tv_usec;
    int64_t endusec = end.tv_sec*1000000 + end.tv_usec;

    double elapsed = (double)(endusec - startusec)/1000000.0;
    std::cout<<"sort toke "<<elapsed<<" seconds"<<std::endl;

    // for(int64_t i = 0; i < util::NUMNUM; ++i) {
    //    std::cout<<"snums["<<i<<"] = "<<util::snums[i]<<std::endl;
    // }
    return 0;

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
    } 
    
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
    
    // if(shm_unlink(name) == -1) {
    //    perror("shm_unlink fail");
    //    return -1;
    // }

    int fd2 = open("/dev/shm/posix_shm2.dat", O_CREAT, S_IRUSR|S_IWUSR);
    if(fd2 == -1) {
        perror("open fail");
        return -1;
    }
    write(fd2, buf, strlen(buf)+1);
    close(fd2);

    return 0;
}

int util::test_mmap(const char *fromfile, const char *tofile) {
    struct stat buf;
    if(stat(fromfile, &buf) != 0) {
        perror("stat");
        return -1;
    }
    
    int64_t st_size = buf.st_size;
    if(st_size == 0) {
        perror("empty file");
        return -1;
    }
    
    int fd = open(fromfile, O_RDONLY);
    if(fd == -1) {
        perror("open fail");
        return -1;
    }

    void *ptr = mmap(NULL, st_size, PROT_READ, MAP_SHARED, fd, 0);
    if(ptr == MAP_FAILED) {
        perror("mmap fail");
        return -1; 
    }

    close(fd);
    
    std::cout<<"file content:"<<std::endl;
    std::cout<<(char *)ptr;
    
    int fd2 = open(tofile, O_CREAT|O_RDWR|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if(fd2 == -1)  {
        perror("open fail");
        return -1;
    }
    
    if(ftruncate(fd2, st_size+10)  == -1) {
        std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" fail"<<std::endl;
        return -1;
    }

    void *ptr2 = mmap(NULL, st_size+10, PROT_READ|PROT_WRITE, MAP_SHARED, fd2, 0);
    if(ptr2 == MAP_FAILED) {
        std::cout<<__FILE__<<":"<<__LINE__<<" "<<__func__<<" fail";
        std::cout<<std::endl; 
        return -1;
    }
    
    memcpy(ptr2, ptr, st_size); 
    
    //const char *name = "charliezhao";
    //memcpy(ptr2, name, strlen(name)+1);
    if(msync(ptr2, st_size, MS_SYNC) == -1) {
        perror("msync fail");
        return -1;
    }

    if(munmap(ptr, st_size) == -1) {
        perror("munmap fail");
        return -1;
    }
    
    if(munmap(ptr2, st_size+10) == -1) {
        perror("munmap fail");
    }
    
    close(fd2);
    return 0;
}


int main(int argc, char *argv[]) {
    
    std::cout<<"errno = "<<errno<<std::endl;
    return util::test_mmap("/home/charlie/tinyhttpd/tools/dat", "/home/charlie/tinyhttpd/tools/dat3");
    return util::test_msg2(); 
    return util::test_msg();
    return util::thread_test(); 
    
    util::test();
    return 0;
    
    
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

