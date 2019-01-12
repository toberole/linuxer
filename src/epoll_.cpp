//
// Created by Apple on 2019/1/11.
//

#include "epoll_.h"

#include <sys/epoll.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#include "Constant.h"


void setnonblocking(int fd) {
    int opts;
    opts = fcntl(fd, F_GETFL);
    if (opts < 0) {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, opts) < 0) {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}


void startEpollTCPServer() {
    printf("----- startEpollTCPServer -----\n");

    int ret = -1;

    struct epoll_event ev;
    struct epoll_event events[20];

    // 创建epoll描述符
    int epfd = epoll_create(256);

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    // 注意一定要设置server_fd为非阻塞模式
    setnonblocking(server_fd);

    // 设置与要处理的事件相关的文件描述符
    ev.data.fd = server_fd;
    // 设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;
    // 注册epoll事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(server_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    ret = listen(server_fd, 10);
    if (ret < 0) {
        printf("listen error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int nfds = -1;

    char buffer[BUFFER_LEN];

    int temp_fd;

    for (;;) {
        // 等待事件发生 epoll_wait是个阻塞方法
        nfds = epoll_wait(epfd, events, 20, -1);
        if (nfds < 0) continue;

        // 处理所有的事件
        for (int i = 0; i < nfds; ++i) {
            if (events[i].data.fd == server_fd) {//监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接
                client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len);
                if (client_fd < 0) {
                    perror("client connected error\n");
                    exit(1);
                }

                setnonblocking(client_fd);
                char *client_addr_str = inet_ntoa(client_addr.sin_addr);
                printf("client connected client addr: %s\n", client_addr_str);

                // 设置操作client_fd
                ev.data.fd = client_fd;
                ev.events = EPOLLIN | EPOLLET;
                // 注册
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);
            } else if (events[i].data.fd & EPOLLIN) {// 读取操作
                temp_fd = events[i].data.fd;
                if (temp_fd < 0) {
                    continue;
                }

                int read_len = read(temp_fd, buffer, BUFFER_LEN);
                if (read_len < 0) {
                    if (errno == ECONNRESET) {
                        printf("read error: %s(errno: %d)\n", strerror(errno), errno);
                        close(temp_fd);
                        events[i].data.fd = -1;
                    } else {
                        printf("read error: %s(errno: %d)\n", strerror(errno), errno);
                    }
                } else if (read_len == 0) {
                    close(temp_fd);
                    events[i].data.fd = -1;
                } else {
                    printf("client data: %s\n", buffer);

                    // 读写交替

                    // 设置用于写的文件描述符
                    // ev.data.fd = temp_fd;
                    // ev.events = EPOLLOUT | EPOLLET;
                    // 修改该fd上要处理的事件为EPOLLOUT
                    // epoll_ctl(epfd,EPOLL_CTL_MOD,temp_fd,&ev);
                }
            } else if (events[i].events & EPOLLOUT) {// 发送数据
                temp_fd = events[i].data.fd;
                char *data = "hello client\n";
                write(temp_fd, data, sizeof(data));

                // 读写交替

                // 设置用于写的文件描述符
                // ev.data.fd = temp_fd;
                // ev.events = EPOLLIN | EPOLLET;
                // 修改该fd上要处理的事件为EPOLLOUT
                // epoll_ctl(epfd,EPOLL_CTL_MOD,temp_fd,&ev);
            }
        }
    }
}
