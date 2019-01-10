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

#define SERVER_PORT 8000
#define MAX_CLIENT_BUFFER 512


void startTCPClient(char *server_ip) {
    int socketfd;
    struct sockaddr_in servaddr;

    char recvline[MAX_CLIENT_BUFFER];

    // 创建socket
    if ((socketfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);

    // inet_pton IP地址转换函数，将IP地址在“点分十进制”和“整数”之间转换
    char *ip = "10.129.29.16";
    int res = inet_pton(AF_INET, ip, &servaddr.sin_addr);
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
    int len = recv(socketfd, recvline, MAX_CLIENT_BUFFER, 0);
    printf("recv len %d\n", len);

    if (len < 0) {
        perror("recv error");
        exit(1);
    }

    close(socketfd);
}

void startUDPClient() {

}