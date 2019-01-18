//
// Created by linux on 19-1-12.
//
#include "Poll_.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

void test_poll() {
    int ret = -1;
    int serv_fd = -1;

    serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_fd < 0) {
        printf("socket errno: %d,errmsg: %s\n", errno, strerror(errno));
        return;
    }

    // 设置非阻塞

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = htons(8000);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    socklen_t len = sizeof(server_addr);

    ret = bind(serv_fd, (struct sockaddr *) &server_addr, len);
    if (ret != 0) {
        printf("bind errno: %d,errmsg: %s\n", errno, strerror(errno));
        return;
    }

    ret = listen(serv_fd, 10);
    if (ret != 0) {
        printf("listen errno: %d,errmsg: %s\n", errno, strerror(errno));
        return;
    }

    // int poll(struct pollfd *fds, nfds_t nfds, int timeout);
    // 设置serv_fd 非阻塞
    int flags = fcntl(serv_fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(serv_fd, F_SETFL, flags);

    struct pollfd fds[11];
    fds[0].fd = serv_fd;
    fds[0].events = POLLIN;


    while (1{
        int client_fd = accept(serv_fd, NULL, NULL);
        if (client_fd <= 0) {
            printf("accept errno: %d,errmsg: %s\n", errno, strerror(errno));
            return;
        }

        char buffer[512];
        memset(buffer, 0, 512);
        ret = read(client_fd, buffer, 512);
        if (ret < 0) {
            printf("read errno: %d,errmsg: %s\n", errno, strerror(errno));
            return;
        }

        printf("client data: %s\n", buffer);

        write(client_fd, buffer, ret);
    }



    close(client_fd);
    close(serv_fd);


}