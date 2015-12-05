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


int accept_request(int client) {
    char buf[1024];
    memset(buf, 0, 1024);
    get_line(client, buf, 1024); 
    fprintf(stdout, "%s", buf);
    sync();
}

int main(int argc, char **argv) {
    settings_init();
    slabs_init(1<<29, 2, true);    
    assoc_init(0); 

    slabs_info();
    item_test();
    
    return 0;

    pthread_t lru;
    int err = pthread_create(&lru, NULL, lru_traverse, NULL);

    struct sockaddr_in cliaddr;
    int server_sock = startup(settings.port);
    if(server_sock < 0) {
        return -1;
    }
    make_socket_non_blocking(server_sock);
    
    int eventfd = epoll_create1(0); 
    
    struct epoll_event event;
    event.data.fd = server_sock;
    event.events = EPOLLIN;
    epoll_ctl(eventfd, EPOLL_CTL_ADD, server_sock, &event);
    
    struct epoll_event *events = (struct epoll_event *)calloc(MAXEVENTS, sizeof(epoll_event));
    
    while(true) {
        int n = epoll_wait(eventfd, events, MAXEVENTS, -1);
        for(int i = 0; i < n; i++) {
            if((events[i].events&EPOLLERR) ||
               (events[i].events&EPOLLHUP)) {
                   fprintf(stderr, "epoll error");
                   epoll_ctl(eventfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]); 
                   close(events[i].data.fd);
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
                event.data.fd = client_sock;
                event.events = EPOLLIN;
                if(epoll_ctl(eventfd, EPOLL_CTL_ADD, client_sock, &event) < 0) {
                    fprintf(stderr, "epoll_ctl fail");
                    abort(); 
                }
                make_socket_non_blocking(client_sock);

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
                char buf[1024];
                memset(buf, 0, 1024); 
                if(get_line(events[i].data.fd, buf, 1024) < 0) {
                   epoll_ctl(eventfd, EPOLL_CTL_DEL, events[i].data.fd, &events[i]); 
                   close(events[i].data.fd);
                }
                else {
                    char key[1024] = {"\0"};
                    char value[1024] = {"\0"};

                    // fprintf(stdout, "%s\n", buf); 
                    // continue;
                    if(strncmp(buf, "get ", 4) == 0) {
                        strcpy(key, buf+4); 
                        // fprintf(stdout, ">get key:%s\n", key);
                        const size_t nkey = strlen(key) + 1;
                        uint32_t cur_hv = jenkins_hash((void *)key, nkey);
                        item *it = assoc_find(key, nkey, cur_hv);
                        if(it == NULL) {
                            fprintf(stderr, "<\033[31mget fail\033[0m\n");
                            size_t rlen = sprintf(buf, "<\033[31mget fail\033[0m\n");
                            if(send(events[i].data.fd, buf, rlen+1, 0) < 0) {
                                fprintf(stdout, "<\033[32msend fail\033[0m\n");
                            } 
                            continue;
                        }
                        // fprintf(stdout, "<\033[32mget ok, %s %s\033[0m\n", ITEM_key(it), ITEM_data(it));
                        size_t rlen = sprintf(buf, "<\033[32mget ok, key:%s value:%s\033[0m\n", ITEM_key(it), ITEM_data(it));
                        if(send(events[i].data.fd, buf, rlen+1, 0) < 0) {
                            fprintf(stderr, "<\033[31msend fail\033[0m\n");
                        }
                    }
                    else if(strncmp(buf, "set ", 4) == 0) { 
                        int offset = 4;
                        for(char *obj=key, *src=buf+offset; 
                            *src != ' '; 
                            *obj++=*src++, ++offset);

                        strcpy(value, buf+offset);
                        
                        // fprintf(stdout, ">set key:%s value:%s\n", key, value); 
                        
                        const size_t nkey = strlen(key) + 1;
                        const int flags = 0;
                        const time_t exptime = 0;
                        const int nbytes = 120;
                        uint32_t cur_hv = jenkins_hash((void *)key, nkey);
                        item *it = do_item_alloc(key, nkey, flags, exptime, nbytes, cur_hv);
                        if(it == NULL) {
                            fprintf(stderr, "<\033[31set fail\033[0m\n");
                            size_t rlen = sprintf(buf, "<\033[31set fail\033[0m\n");
                            if(send(events[i].data.fd, buf, rlen+1, 0) < 0) {
                                fprintf(stderr, "<\033[31msend fail\033[0m\n");
                            }
                            continue; 
                        }
                        memcpy(ITEM_data(it), value, strlen(value)+1);
                        
                        size_t rlen = sprintf(buf, "<\033[32mset ok, key:%s value:%s\033[0m\n", key, value);
                        if(send(events[i].data.fd, buf, rlen+1, 0) < 0)
                        {
                            fprintf(stdout, "<\033[32msend fail\033[0m\n");
                        }

                    }
                    else {
                        fprintf(stdout, "<%s ,format error, ignore!\n", buf);
                    }

                }
            }
        }
    }
    free(events);
    pthread_join(lru, NULL);
    sleep(600);
    return 0;
}
