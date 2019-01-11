//
// Created by Apple on 2019/1/9.
//

#include "Client.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "Constant.h"


void startTCPClient(const char *server_ip) {
    printf("--- startTCPClient --- ip:%s\n", server_ip);

    int socketfd;
    struct sockaddr_in servaddr;

    char recvline[BUFFER_LEN];

    // 创建socket
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    // inet_pton IP地址转换函数，将IP地址在“点分十进制”和“整数”之间转换
    int res = inet_pton(AF_INET, server_ip, &servaddr.sin_addr);
    if (res != 1) {
        printf("inet_pton error\n");
        exit(0);
    }

    // 连接
    int connect_res = connect(socketfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
    if (connect_res != 0) {
        printf("connect server error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 发送数据
    char *data = "hello world";
    int send_data_res = send(socketfd, data, strlen(data), 0);
    printf("send_data_res: %d\n", send_data_res);
    if (send_data_res < 0) {
        printf("send data error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 接收数据
    int len = recv(socketfd, recvline, BUFFER_LEN, 0);
    printf("recv len %d\n", len);

    if (len < 0) {
        perror("recv error");
        exit(1);
    }

    close(socketfd);
}

void startUDPClient(const char *server_ip) {
    printf("--- startUDPClient --- ip:%s\n", server_ip);

    int client_fd;
    struct sockaddr_in serv_addr;

    // 第三个参数传0 表示protocol 由前两个参数组合确定
    client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_fd == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 设置内核UDP缓冲区大小
    // int n = 100 * 1024;
    // setsockopt(client_fd, SOL_SOCKET, SO_RCVBUF,&n, sizeof(n));

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr.s_addr) != 1) {
        printf("inet_pton error\n");
        exit(0);
    }

    char *sendData = "hello server";

    socklen_t len = sizeof(serv_addr);
    int send_res = sendto(client_fd, sendData, strlen(sendData), 0, (struct sockaddr *) &serv_addr, len);
    if (-1 == send_res) {
        printf("upd send data error: %s (errno: %d)\n", strerror(errno), errno);
        close(client_fd);
        exit(0);
    }


    char buffer[BUFFER_LEN];
    memset(buffer, 0, BUFFER_LEN);
    struct sockaddr_in src_addr;
    int recv_len = recvfrom(client_fd, buffer, BUFFER_LEN, 0, (struct sockaddr *) &src_addr, &len);
    if (recv_len < 0) {
        printf("upd recv error: %s (errno: %d)\n", strerror(errno), errno);
        close(client_fd);
        exit(0);
    }

    char *data = (char *) malloc(recv_len * sizeof(char) + 1);
    memset(data, 0, recv_len + 1);
    memcpy(data, buffer, recv_len);

    printf("server data: %s\n", data);

    close(client_fd);
}