#include "head.h"
#define BUFLEN 1024
#define MAX_EVENTS 1024

const bool debug = true;

void usage(const char *name);
void aiousage(const char *name);

class TcpServer {
    public:
        TcpServer(uint32_t port);
        ~TcpServer();

        int epollrun(void); 

    private:
        int domain_;
        int type_;
        int protocol_ = 0;
        uint32_t port_;

        int sfd_ = 0;
};


class Command {
    public:
        Command(const char *command):command(command) {}
        ~Command() {fclose(fp);}

        void run(void);

    private:
        const int len = 1024;
        const char *command;
        FILE *fp;
        const char * const mode = "r";
};

class BatchCommand {
    public:
        BatchCommand(int cnt, char **commands)
        {
            for(int i = 1; i < cnt; ++i) {
                cmds.push(*(commands+i));
            }
        }

        ~BatchCommand() {}
        int run(void); // use multi thread.
        int epoll_run(void); // use epoll.
        int select_run(void); // use select.
        void test(void);

    private:
        int _run(void);
        std::stack<const char *> cmds;
        std::mutex mutex_;
};

class AsyncIO {
    public:
        AsyncIO(int argc, char **argv);
        ~AsyncIO(void);
        void test(void);

    private:
        std::vector<const char *> filenames_;
};

#define BUF_SIZE 102400
#define errExit(msg) do {perror(msg); exit(EXIT_FAILURE); } while(false)
#define errMsg(msg) do {perror(msg); } while(false)

struct ioRequest {
    int reqNum;
    int status;
    struct aiocb *aiocbp;
};

static volatile sig_atomic_t  gotSIGQUIT = 0;

static void quitHandler(int signo) {
    gotSIGQUIT = 1;
}

#define IO_SIGNAL SIGUSR1

static void aioSigHandler(int sig, siginfo_t *si, void *ucontext) {
//static void aioSigHandler(int sig) {
    write(STDOUT_FILENO, "I/O completion signal received\n", 31);
}

int aiotest(int argc, char **argv);
