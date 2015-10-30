#include "head.h"

class Agent {
    public:
        Agent(const char *ip, int port):ip_(ip), port_(port) {}
        ~Agent() {}
        int test_one();
        static void connect_alarm(int signo);
    private:
        const char *ip_;
        int port_ = 0;

        int domain_ = AF_INET;
        int type_ = SOCK_STREAM;
        int fd_ = 0;
};

void Agent::connect_alarm(int signo) {
    std::cout<<"signal "<<signo<<" catch"<<std::endl;
}

void  connect_alarm_g(int signo) {
    std::cout<<"signal "<<signo<<" catch"<<std::endl;
    exit(0);
}

int Agent::test_one() {
beg:
    if((fd_ = socket(domain_, type_, 0)) == -1) {
        perror("socket fail");
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_);
    addr.sin_port=htons(port_);

    // deal connect timeout
    struct sigaction act, oact;
    act.sa_handler = connect_alarm_g;
    sigemptyset(&act.sa_mask);
    
    #ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
    #endif
    
    if(sigaction(SIGALRM, &act, &oact) < 0) {
        perror("SIGALRM fail");
    }
    alarm(1); 
    
    // do connect
    int ret = connect(fd_, (const struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("connect fail");
        return -1;
    }
    
    //restore alarm 
    alarm(0);
    if(sigaction(SIGALRM, &oact, NULL) < 0) {
        perror("SIGALRM fail");
    }
    
    //send data
    struct s_args args;
    while(true) {
        args.i = 0x1234;
        args.j = 0x5678;
        args.k = 0x9abc;
        args.m = 0xde01;
        
        int ret = send(fd_, (const void *)&args, sizeof(args), 0);
        if(ret == -1) {
            perror("send fail");
            goto beg; 
        }
        std::cout<<"send ok"<<std::endl;
        sleep(1);
    }
    return 0;
}


int main(int argc, char *argv[]) {
    
    const char *ip = "127.0.0.1";
    int port = 9999;
    Agent inst(ip, port);
    inst.test_one();

    return 0;
}
        
