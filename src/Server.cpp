//
// Created by Apple on 2019/1/9.
//
#include "Server.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <thread>

#include <AppUtil.h>

#define DEFAULT_PORT 8000
#define BUFFER_LENGTH 512

bool switch_server = false;

void reader_writer(int connect_fd) {
    // 接受客户端传过来的数据
    char buffer[BUFFER_LENGTH];
    memset(buffer, 0, BUFFER_LENGTH);

    int len = -1;

    while (1) {
        len = recv(connect_fd, buffer, BUFFER_LENGTH, 0);
        if (len > 0) {
            char *data = (char *) malloc(len * sizeof(char) + 1);
            if (data == nullptr) continue;

            memset(data, 0, BUFFER_LENGTH);
            memcpy(data, buffer, len);

            // 移除空白字符
            trim(data,len);

            printf("客户端数据: %d -- %s \n",strlen(data),data);

            if (strcmp(data, "cls") == 0) {
                close(connect_fd);
                delete data;
                break;
            }

            int res = send(connect_fd, data, len, 0);
            if (res == -1) {
                perror("send data error");
                close(connect_fd);
                break;
            } else {
                delete data;
            }


        } else {
            close(connect_fd);
            break;
        }
    }
}

// TCP 服务端
void startTCPServer() {
    // 服务端的fd
    int socket_fd;

    // 连接上来的客户端的fd
    int connect_fd;

    struct sockaddr_in servaddr;


    // 初始化socket
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == socket_fd) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 初始化serveradd
    memset(&servaddr, 0, sizeof(sockaddr_in));
    servaddr.sin_family = AF_INET;
    // INADDR_ANY 系统自动获取本机地址
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 设置端口号
    servaddr.sin_port = htons(DEFAULT_PORT);

    // bind 绑定套接字
    if (bind(socket_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 监听
    if (listen(socket_fd, 10) == -1) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    switch_server = true;

    // 接受客户端的连接
    while (switch_server) {

        if ((connect_fd = accept(socket_fd, (struct sockaddr *) NULL, NULL)) == -1) {
            printf("accept socket error: %s(errno: %d)\n", strerror(errno), errno);
            continue;
        }

        printf("客户端连接上来 \n");

        std::thread rw(reader_writer, connect_fd);
        rw.detach();
    }
}


void startUDPServer() {

}
