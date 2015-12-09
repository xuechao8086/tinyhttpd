/*************************************************************************
	> File Name:    main.cpp
	> Author:       charliezhao 
	> Mail:         xuechao8086@126.com 
	> Created Time: Thu 03 Dec 2015 05:59:19 PM PST
 ************************************************************************/

#include "memcache.h"

extern struct conf settings;

static int make_socket_non_blocking(int sfd);

volatile time_t current_time;
static int max_fds;
static int startup(int port);

int event_fd = 0;
conn **conns;
static void conn_init(void);
static conn *conn_new(const int sfd, const int read_buffer_size, 
                      const char *cip, size_t ncip,
                      const char *cport, size_t ncport);
static void conn_cleanup(conn *c);

static enum try_read_result try_read_network(conn *c);
static int try_write_network(conn *c);
static int write_wbuffer(conn *c, const void *ptr, size_t len);

static int process_request(conn *c);
static item * process_request_get(conn *c, const char *key, size_t nkey);
static int process_request_set(conn *c, const char *key, size_t nkey,
                               const char *val, size_t nval, int exptime);

static int startup(int port) {
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
    if(listen(httpd, settings.backlog) < 0) {
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

static conn *conn_new(const int sfd, const int read_buffer_size, 
                      const char *cip, size_t ncip,
                      const char *cport, size_t ncport) {
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
    memset(c->rbuf, 0, c->rsize);
    c->wbuf = (char *)malloc((size_t)c->wsize);
    memset(c->wbuf, 0, c->wsize); 
    c->rbytes = c->wbytes = 0;
    c->rcurr = c->rbuf;
    c->wcurr = c->wbuf;

    if (c->rbuf == 0 || c->wbuf == 0 ) {
        fprintf(stderr, "Failed to allocate buffers for connection\n");
        free(c);
        return NULL; 
    }

    c->sfd = sfd;
    memset(c->cip, 0, NI_MAXHOST);
    strncpy(c->cip, cip, ncip);
    c->cip[NI_MAXHOST-1] = '\0';   

    memset(c->cport, 0, NI_MAXSERV);
    strncpy(c->cport, cport, ncport);
    c->cport[NI_MAXSERV-1] = '\0';    

    conns[sfd] = c;
   
    struct epoll_event event;
    memset((void *)&event, 0, sizeof(event));
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
        memset((void *)&event, 0, sizeof(event)); 
        event.data.fd = c->sfd;
        event.events = EPOLLIN;
        if(epoll_ctl(event_fd, EPOLL_CTL_DEL, c->sfd, &event) < 0) {
            fprintf(stdout, "epoll_ctl fail");
        }
        close(c->sfd);

        fprintf(stdout, "%s:%d %s close connnection on %d(%s:%s)\n", 
                __FILE__, __LINE__, __func__,
                c->sfd, c->cip, c->cport);
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
    if(settings.verbose > 1) {
        // fprintf(stdout, "%s:%d %s buf = %s\n",
        //         __FILE__, __LINE__, __func__,
        //         c->rbuf);
    }
    return gotdata; 
}

static int write_wbuffer(conn *c, const void *ptr, size_t len) {
    assert(c != NULL);
    
    int avail = c->wsize - c->wbytes;
    int num_allocs = 0; 
    if(avail > len) {
        memcpy(c->wcurr, ptr, len);
        c->wcurr += len;
        c->wbytes += len;
    }
    else {
        int ret = try_write_network(c);
        if(ret != 0) {
            return ret;
        }

        if(c->wsize < len) {
            c->wbuf = realloc(c->wbuf, 2*len);
            memset(c->wbuf, 0, 2*len);
            c->wcurr = c->wbuf;
            c->wsize = 2*len;
            c->wbytes = 0;
        }
        memcpy(c->wcurr, ptr, len);
        c->wcurr += len;
        c->wbytes += len;
    }
    return 0;
}

static int try_write_network(conn *c) {
    assert(c != NULL);
    
    int cnt = 0;
    while(cnt < c->wbytes) {
        int ret = write(c->sfd, c->wbuf+cnt, c->wbytes-cnt);
        if(ret >= 0) {
            cnt += ret;
        } 
        else {
            if((errno == EAGAIN) || (errno = EWOULDBLOCK)) {
                continue;
            }
            else {
                fprintf(stderr, "\033[31mwrite fail\033[0m\n");
                break;
            }
        } 
    }
    c->wcurr -= cnt;
    c->wbytes -= cnt;
    return 0;
}

static int process_request(conn *c) {
    assert(c != NULL);

    char *beg = c->rcurr;
    char *pos = c->rcurr;
    
    char buf[1024] = {"\0"};

    int i = 0;
    int j = 0;
    int maxpoint = 0;
    while(j < c->rbytes ) {
        if(*pos == ';') {
            strncpy(buf, c->rcurr, i);
            buf[i] = '\0';
            strip_blank(buf, i);
            c->rcurr = pos + 1;
            maxpoint += i+1;
            i = 0;
            if(buf[0] != '\0') {
                if(settings.verbose > 1) {
                    // fprintf(stdout, "%s:%d %s buf = %s\n",__FILE__, __LINE__, __func__, buf);
                }
                
                char *token[10];
                memset((void *)token, 0, sizeof(char *));
                int k = 0;
                char *tp = buf; 
                while((token[k] = strtok(tp, " ")) != NULL) {
                    ++k;
                    tp = NULL;
                }
                
                int maxtoken = k;
                for(int l = 0; l < maxtoken; ++l) {
                    // fprintf(stdout, "%s\n", token[l]);
                }
                
                if(strncmp(token[0], "get", 3) == 0) {
                    // process get
                    if(maxtoken < 2) {
                        fprintf(stderr, "%s:%d %s buf = %s\n", __FILE__, __LINE__, __func__, buf);
                    }
                    else {
                        const char *key = token[1];
                        size_t nkey = strlen(key);
                        if(settings.verbose > 1) {
                        }
                        item *it = process_request_get(c, key, nkey);
                        if(it == NULL) {
                            fprintf(stdout, "%s:%d %s cmd:%s key:%s not found\n", 
                                    __FILE__, __LINE__, __func__, 
                                    token[0], key);
                        }
                        
 
                        char r[1024] = {"\0"};
                        sprintf(r, "%s:%d %s cmd:%s key:%s val:%s ret:0\n",
                                __FILE__, __LINE__, __func__, 
                                token[0], key, ITEM_data(it));
                        if(settings.verbose > 1) {
                            fprintf(stdout, "%s", r); 
                        }
                        // write_wbuffer(c, r, strlen(r)+1); 
                        write(c->sfd, r, strlen(r)+1);
                    } 
                }
                else if(strncmp(token[0], "set", 3) == 0) {
                    // process set
                    if(maxtoken < 3) {
                        fprintf(stderr, "%s:%d %s buf = %s\n", __FILE__, __LINE__, __func__, buf);
                    }
                    else {
                        const char *key = token[1];
                        size_t nkey = strlen(key);
                        const char *val = token[2];
                        size_t nval = strlen(val);

                        int ret = process_request_set(c, key, nkey, val, nval, 0);
                        
                        char r[1024] = {"\0"};
                        sprintf(r, "%s:%d %s cmd:%s key:%s value:%s ret:%d\n",
                                __FILE__, __LINE__, __func__,
                                token[0], key, val, ret);
                        if(ret == 0) {
                            if(settings.verbose > 1) {
                                fprintf(stdout, "%s", r); 
                            }
                        }
                        else {
                            fprintf(stdout, "%s", r); 
                        }
                        write(c->sfd, r, strlen(r)+1);
                    }
                } 
                else {
                    fprintf(stderr, "%s:%d %s buf = %s, format error", 
                            __FILE__, __LINE__, __func__, 
                            buf);
                }
            } 
        }
        else {
            ++i;
        }
        
        ++pos;
        ++j;
    }
    
    c->rbytes -= maxpoint;
    return 0;
}

static item * process_request_get(conn *c, const char *key, size_t nkey) {
    uint32_t hv = jenkins_hash((void *)key, nkey);
    return assoc_find(key, nkey, hv);
}

static int process_request_set(conn *c, const char *key, size_t nkey,
                               const char *val, size_t nval, int exptime)
{ 
    uint32_t hv = jenkins_hash((void *)key, nkey);
    item *it = assoc_find(key, nkey, hv);
     
    int flags = 0; // later consider what's the mean of flags?
    
    if(it == NULL) {
        item *it = do_item_alloc(key, nkey, flags, exptime, nval, hv);
        if(it == NULL) {
            return -1; 
        }
        memcpy(ITEM_data(it), val, nval);
        return 0;
    }    
    
    //how to update? 
    if(it->nbytes == nval) {
        int ret = memcmp(val, ITEM_data(it), nval);
        if(ret == 0) {
            return 0;
        }
        memcpy(ITEM_data(it), val, nval);
    }
    else if (it->nbytes < nval){
        do_item_unlink(it, hv);
        item *it = do_item_alloc(key, nkey, flags, exptime, nval, hv);
        if(it == NULL) {
            return -1;
        }
        memcpy(ITEM_data(it), val, nval);
    }
    else {
        memset(ITEM_data(it), 0, it->nbytes);
        memcpy(ITEM_data(it), val, nval);
    }

    return 0;
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
    memset((void *)&event, 0, sizeof(event)); 
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

                char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
                
                int s = getnameinfo((struct sockaddr *)&cliaddr, cli_len,
                                    hbuf, sizeof(hbuf),
                                    sbuf, sizeof(sbuf),
                                    NI_NUMERICHOST | NI_NUMERICSERV);
                if(s == 0) {
                    fprintf(stdout, "%s:%d %s accepted connection on descriptor %d(%s:%s)\n", 
                            __FILE__, __LINE__, __func__,
                            client_sock, hbuf, sbuf);
                }    
                
                conn *c = conn_new(client_sock, 1024, hbuf, NI_MAXHOST, sbuf, NI_MAXSERV);
                if(c == NULL) {
                    break;
                } 
            }
            else {
                conn *c = conns[events[i].data.fd];
                if(try_read_network(c) != READ_DATA_RECEIVED) {
                    conn_cleanup(c);
                    continue; 
                }
                process_request(c);
            }
        }
    }
    free(events);
    // pthread_join(lru, NULL);
    sleep(600);
    return 0;
}
