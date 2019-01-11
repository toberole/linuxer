//
// Created by Apple on 2019/1/10.
//


/*
 * int select(int nfds,
 *            fd_set *restrict readfds,
 *            fd_set *restrict writefds,
 *            fd_set *restrict errorfds,
              struct timeval *restrict timeout);
 */

#include "Select.h"


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

#include "AppUtil.h"
#include "Constant.h"


void test_select() {
    fd_set rd;
    struct timeval tv;
    int err;

    FD_ZERO(&rd);
    FD_SET(0, &rd);

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    err = select(1, &rd, nullptr, nullptr, &tv);

    if (0 == err) {
        printf("select time out\n");
    } else if (-1 == err) {
        printf("fail to select\n");
    } else {
        printf("data is avaiable\n");
    }

}

/**
 * 服务端与一个客户端的读写操作采用select模式
 */
void startSelectTCPServer() {
    int server_fd;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));

    addr.sin_port = htons(SERVER_PORT);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("create socket error\n");
    }

    int bind_res = bind(server_fd, (struct sockaddr *) &addr, sizeof(addr));
    if (-1 == bind_res) {
        printf("bind error\n");
    }

    int listen_res = listen(server_fd, 10);
    if (listen_res == -1) {
        printf("listen error\n");
    }

    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t len_sock = sizeof(client_addr);
    int connected_fd = accept(server_fd, (struct sockaddr *) &client_addr, &len_sock);

    printf("----------- accept ---------\n");

    if (connected_fd < 0) {
        printf("client connect error\n");
    }

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    fd_set read_fds;
    fd_set exception_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&exception_fds);

    while (1) {
        /* 每次调用select之前都要重新在read_fds和exception_fds中设置文件描述符connected_fd
         * 因为事件发生以后，文件描述符集合将被内核修改
         * */

        memset(buffer, 0, sizeof(buffer));

        FD_SET(connected_fd, &read_fds);
        FD_SET(connected_fd, &exception_fds);

        int res = select(connected_fd + 1, &read_fds, nullptr, &exception_fds, NULL);
        if (res < 0) {
            printf("select error\n");
            break;
        }

        // 判断 connected_fd 是否可读
        if (FD_ISSET(connected_fd, &read_fds)) {
            res = recv(connected_fd, buffer, sizeof(buffer) - 1, 0);
            if (res < 0) {
                printf("recv error: %s(errno: %d)\n", strerror(errno), errno);
                break;
            } else {
                trim(buffer, res);

                printf("get %d bytes of normal data: %s \n", res, buffer);

                if (strcmp(buffer, "stop") == 0) {
                    break;
                }
            }
        } else if (FD_ISSET(connected_fd, &exception_fds)) {
            res = recv(connected_fd, buffer, sizeof(buffer) - 1, MSG_OOB);
            if (res <= 0) {
                break;
            }
            printf("get %d bytes of exception data: %s \n", res, buffer);
        }
    }

    close(connected_fd);
    close(server_fd);
}


/**
 *
 * 与客户端的读写操作和accept操作全部采用 select
 */
void startSelectTCPServerN2N() {
    int ret = -1;

    int serv_fd;
    int client_fd;

    char buffer[BUFFER_LEN];
    memset(buffer, 0, BUFFER_LEN);

    fd_set read_fds;
    fd_set temp_fds;
    FD_ZERO(&read_fds);
    FD_ZERO(&temp_fds);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    ret = bind(serv_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (ret < 0) {
        printf("bind socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }

    ret = listen(serv_fd, 10);
    if (ret < 0) {
        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
        return;
    }


    FD_SET(serv_fd, &read_fds);//将服务器端socket加入到集合中

    while (1) {
        //将需要监视的描述符集copy到select查询队列中，select会对其修改，所以一定要分开使用变量
        temp_fds = read_fds;

        printf("------ select before ------\n");

        // FD_SETSIZE 系统默认的最大文件描述符
        ret = select(FD_SETSIZE, &temp_fds, nullptr, nullptr, nullptr);

        if (ret < 0) {
            printf("------ select error ------\n");
            continue;
        }

        // 扫描所有的文件描述符
        for (int fd = 0; fd < FD_SETSIZE; ++fd) {
            if (FD_ISSET(fd, &temp_fds)) {
                // 判断是否是服务器套接字
                if (fd == serv_fd) {// 客户端请求连接
                    struct sockaddr_in client_addr;
                    memset(&client_addr, 0, sizeof(client_addr));
                    socklen_t len_sock = sizeof(client_addr);
                    client_fd = accept(serv_fd, (struct sockaddr *) &client_addr, &len_sock);
                    if (client_fd < 0) {
                        printf("listen socket error: %s(errno: %d)\n", strerror(errno), errno);
                    } else {
                        //将客户端socket加入到集合中
                        FD_SET(client_fd, &read_fds);
                        printf("adding client on fd %d\n", client_fd);
                    }
                } else {// 客户端读写
                    /**
                        ioctl是设备驱动程序中对设备的I/O通道进行管理的函数。所谓对I/O通道进行管理，就是对设备的一些特性进行控制，例如串口的传输波特率、马达的转速等等。
                        它的调用个数如下：int ioctl(int fd, ind cmd, …)；
                        其中fd是用户程序打开设备时使用open函数返回的文件标示符，cmd是用户程序对设备的控制命令，
                        至于后面的省略号，那是一些补充参数，一般最多一个，这个参数的有无和cmd的意义相关。
                        ioctl函数是文件结构中的一个属性分量，就是说如果你的驱动程序提供了对ioctl的支持，
                        用户就可以在用户程序中使用ioctl函数来控制设备的I/O通道。
                     */
                    int recv_len;
                    // FIONREAD 获取接收缓存区中的字节数命令
                    ioctl(fd, FIONREAD, &recv_len);

                    if (recv_len == 0) {// 客户端请求完毕
                        // 关闭套接字 移除fd
                        close(fd);
                        FD_CLR(fd, &read_fds);
                        printf("removing client on fd %d\n", fd);
                    } else {
                        memset(buffer, 0, BUFFER_LEN);
                        // 读取客户端数据
                        read(fd, buffer, recv_len);
                        trim(buffer, recv_len);
                        printf("client data: %s\n", buffer);

                        // 回复客户端数据
                        char *data = "hello client";
                        write(fd, data, strlen(data));
                    }
                }
            }
        }
    }
}


