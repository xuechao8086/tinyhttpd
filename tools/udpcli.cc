#include "head.h"

int main(int argc, char *argv[]) {
    util::daemon();
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8888);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    
    char buf[1024] = "\0";
   
    int i = 0;
    while(true) {
        int fd = socket(AF_INET, SOCK_DGRAM, 0);
        connect(fd, (struct sockaddr *) &addr, sizeof(addr));
        sprintf(buf, "from client, seq = %d\n", i++);
        send(fd, buf, sizeof(buf), 0);
        std::cout<<buf<<" send ok"<<std::endl;
        close(fd);
        //sleep(1);
    }
}
