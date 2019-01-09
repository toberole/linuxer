//
// Created by Apple on 2019/1/9.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <thread>

#define DEFAULT_PORT 8000
#define MAXLINE 4096


void startServer() {
    int socket_fd;
    int connect_fd;
    struct sockaddr_in servadd;
    char buffer[MAXLINE];
    memset(buffer, 0, MAXLINE);

    // 初始化socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == socket_fd) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 初始化serveradd
    memset(&servadd, 0, sizeof(sockaddr_in));
    servadd.sin_family = AF_INET;
    // INADDR_ANY 系统自动获取本机地址
    servadd.sin_addr.s_addr = htonl(INADDR_ANY);
    // 设置端口号
    servadd.sin_port = htons(DEFAULT_PORT);

    // bind 绑定套接字
    if (bind(socket_fd, (struct sockaddr *) &servadd, sizeof(servadd)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 监听
    if (listen(socket_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    int n = -1;

    // 接受客户端的连接
    while (1) {
        if ((connect_fd = accept(socket_fd, (struct sockaddr *) NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            continue;
        }

        // 接受客户端传过来的数据
        n = recv(connect_fd, buffer, MAXLINE, 0);
        if (n > 0) {
            char *data = (char *) malloc(n * sizeof(char) + 1);
            memset(data, 0, MAXLINE);
            memcpy(data, buffer, n);

            printf("客户端数据： %s\n",data);

            close(connect_fd);
        }

    }
}