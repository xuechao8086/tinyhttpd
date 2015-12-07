/*************************************************************************
	> File Name:    main.cpp
	> Author:       charliezhao 
	> Mail:         xuechao8086@126.com 
	> Created Time: Thu 03 Dec 2015 05:59:19 PM PST
 ************************************************************************/

#include "memcache.h"

extern struct settings settings;
static int make_socket_non_blocking(int sfd);

volatile time_t current_time;
static int max_fds;

int event_fd = 0;
conn **conns;
static void conn_init(void);
static conn *conn_new(const int sfd, const int read_buffer_size);
static void conn_cleanup(conn *c);

static enum try_read_result try_read_network(conn *c);

int startup(int port) {
    int httpd = socket(AF_INET, SOCK_STREAM, 0);
    if(httpd == -1) {
        return -1; 
    }
    struct sockaddr_in name;
    memset(&name, 0, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int reuse_addr = 1;
    setsockopt(httpd, SOL_SOCKET, SO_REUSEPORT,
               (const char*)&reuse_addr, sizeof(reuse_addr));
    if(bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        return -1;
    }
    if(listen(httpd, SOMAXCONN) < 0) {
        return -1;
    }
    return httpd;
}

static int make_socket_non_blocking(int sfd) {
    int flags = fcntl(sfd, F_GETFL, 0);
    if(flags == -1) {
        return -1;
    }
    flags |= O_NONBLOCK;
    int s = fcntl(sfd, F_SETFL, flags);
    if (s < 0) {
        return -1;
    }
    return 0;
}


int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n =  recv(sock, buf, size, 0);
    if(n == -1) {
        return -1;
    }
    else if(n == 0) {
        return -1;
    }
    for(int i = 0; i < n; ++i) {
        if(*(buf+i) == '\r' || *(buf+i) == '\n') {
            *(buf+i) = '\0';
        }
    }
    fprintf(stdout, "buf=%s\n", buf);
    return 0; 

    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);

        //which kinds of characters can get here?
        if (-1 == n && errno == EAGAIN){
            // fprintf(stderr, "never be here?\n");
            return -1; 
        }
        else if (n > 0)
        {
            if (c == '\r' || c == '\n')
            {
                buf[i++] = '\0'; 
            }
            buf[i++] = c;
        }
        else {
            break;
        }
    }
    return 0; 
}

static void conn_init(void) {
    int headroom = 10;
    max_fds = settings.maxconns + headroom;

    struct rlimit rl;
    if(getrlimit(RLIMIT_NOFILE, &rl) == 0) {
        max_fds = rl.rlim_max;
    }
    else {
        fprintf(stderr, "Failed to query maximum file descriptor; "
                        "falling back to maxconns\n");
    }
    conns = (conn **)calloc(max_fds, sizeof(conn *));

    if(conns == NULL) {
        fprintf(stderr, "Failed to allocate connection structures\n");
        exit(1);
    }
}

static conn *conn_new(const int sfd, const int read_buffer_size) {
    conn *c = conns[sfd];
    if(NULL == c) {
        if (!(c = (conn *)calloc(1, sizeof(conn)))) {
            fprintf(stderr, "Failed to allocate connection object\n");
            return NULL;
        }
    }

    c->rbuf = c->wbuf = 0;
    c->rsize = read_buffer_size;
    c->wsize = DATA_BUFFER_SIZE;
    c->rbuf = (char *)malloc((size_t)c->rsize);
    c->wbuf = (char *)malloc((size_t)c->wsize);
    c->rbytes = c->wbytes = 0;
    c->rcurr = c->rbuf;
    c->wcurr = c->wbuf;

    if (c->rbuf == 0 || c->wbuf == 0 ) {
        fprintf(stderr, "Failed to allocate buffers for connection\n");
        free(c);
        return NULL; 
    }

    c->sfd = sfd;
    conns[sfd] = c;
   
    struct epoll_event event;
    event.data.fd = sfd;
    event.events = EPOLLIN;
    if(epoll_ctl(event_fd, EPOLL_CTL_ADD, sfd, &event) < 0) {
        fprintf(stderr, "epoll_ctl fail");
        free(c->rbuf);
        free(c->wbuf);
        free(c);
        return NULL;
    }
    return c;
}

static void conn_cleanup(conn *c) {
    if(c != NULL) {
        if(c->rbuf != NULL) {
            free(c->rbuf);
        }
        if(c->wbuf != NULL) {
            free(c->wbuf);
        }
        struct epoll_event event;
        event.data.fd = c->sfd;
        event.events = EPOLLIN;
        if(epoll_ctl(event_fd, EPOLL_CTL_DEL, c->sfd, &event) < 0) {
            fprintf(stdout, "epoll_ctl fail");
        }
        close(c->sfd);

        fprintf(stdout, "close connnection on %d\n", c->sfd);
    } 
}

static enum try_read_result try_read_network(conn *c) {
    assert(c != NULL);
    enum try_read_result gotdata = READ_NO_DATA_RECEIVED;

    if(c->rcurr != c->rbuf) {
        if((c->rbytes != 0)) {
            memmove(c->rbuf, c->rcurr, c->rbytes);
        }
        c->rcurr = c->rbuf;
    }
    
    int num_allocs = 0;
    while(true) {
        if(c->rbytes >= c->rsize) {
            if(num_allocs == 2) {
                return gotdata; 
            }
            ++num_allocs;
            char *new_rbuf = (char *)realloc(c->rbuf, c->rsize*2);
            if(!new_rbuf) {
                fprintf(stderr, "Couldn't realloc input buffer\n");
                return READ_MEMORY_ERROR;
            }
            c->rcurr = c->rbuf = new_rbuf;
            c->rsize *= 2;
        }
        int avail = c->rsize - c->rbytes;
        int res = read(c->sfd, c->rbuf+c->rbytes, avail);
 
        if(res > 0) {
            gotdata = READ_DATA_RECEIVED;
            c->rbytes += res;

            if(res == avail) {continue;}
            else { break; } 
        }
        else if(res == 0) {
            return READ_ERROR;
        }
        else if(res == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            return READ_ERROR;
        }
    }
    fprintf(stdout, "buf = %s\n", c->rbuf);
    return gotdata; 
}


int main(int argc, char **argv) {
    settings_init();
    slabs_init(1<<29, 2, true);    
    assoc_init(0); 
    conn_init();

    // item_test();
    // pthread_t lru;
    // int err = pthread_create(&lru, NULL, lru_traverse, NULL);

    struct sockaddr_in cliaddr;
    int server_sock = startup(settings.port);
    if(server_sock < 0) {
        return -1;
    }
    make_socket_non_blocking(server_sock);
    
    event_fd = epoll_create1(0); 
    
    struct epoll_event event;
    event.data.fd = server_sock;
    event.events = EPOLLIN;
    epoll_ctl(event_fd, EPOLL_CTL_ADD, server_sock, &event);
    
    struct epoll_event *events = (struct epoll_event *)calloc(MAXEVENTS, sizeof(epoll_event));
    
    while(true) {
        int n = epoll_wait(event_fd, events, MAXEVENTS, -1);
        for(int i = 0; i < n; i++) {
            if((events[i].events&EPOLLERR) ||
               (events[i].events&EPOLLHUP)) {
                   fprintf(stderr, "epoll error");
                   conn_cleanup(conns[events[i].data.fd]); 
                   break; 
               }
            else if (server_sock == events[i].data.fd) {
                socklen_t cli_len = sizeof(cliaddr);
                int client_sock = accept(server_sock,
                                         (struct sockaddr *)&cliaddr,
                                         &cli_len);
                if(client_sock == -1) {
                    if(errno == EAGAIN || errno == EWOULDBLOCK) {
                        break;
                    }
                }
                
                make_socket_non_blocking(client_sock);
                conn *c = conn_new(client_sock, 1024);
                if(c == NULL) {
                    break;
                } 

                char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                
                int s = getnameinfo((struct sockaddr *)&cliaddr, cli_len,
                                    hbuf, sizeof(hbuf),
                                    sbuf, sizeof(sbuf),
                                    NI_NUMERICHOST | NI_NUMERICSERV);
                if(s == 0) {
                    fprintf(stdout, "accepted connection on descriptor %d(host=%s port=%s)\n", 
                            client_sock, hbuf, sbuf);
                }    
            }
            else {
                conn *c = conns[events[i].data.fd];
                if(try_read_network(c) != READ_DATA_RECEIVED) {
                    conn_cleanup(c);
                    continue; 
                }
                process_request(conn *c);

            }
        }
    }
    free(events);
    // pthread_join(lru, NULL);
    sleep(600);
    return 0;
}
