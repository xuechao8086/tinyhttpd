#include "head.h"

class Agent {
    public:
        Agent(const char *ip, int port):ip_(ip), port_(port) {}
        ~Agent() {}
        int test_one();
    private:
        const char *ip_;
        int port_ = 0;

        int domain_ = AF_INET;
        int type_ = SOCK_STREAM;
        int fd_ = 0;
};

int Agent::test_one() {
    if((fd_ = socket(domain_, type_, 0)) == -1) {
        perror("socket fail");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip_);
    addr.sin_port=htons(port_);
    int ret = connect(fd_, (const struct sockaddr *)&addr, sizeof(addr));
    if(ret == -1) {
        perror("connect fail");
        return -1;
    }

    struct s_args args;
    while(true) {
        args.i = 0x1234;
        args.j = 0x5678;
        args.k = 0x9abc;
        args.m = 0xde01;
        
        int ret = send(fd_, (const void *)&args, sizeof(args), 0);
        if(ret == -1) {
            perror("send fail");
            continue;
        }
        sleep(60);
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
        
