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

    nfds_t nfds = 10;

    char buffer[512];
    memset(buffer, 0, 512);

    while (1) {
        ret = poll(fds, nfds, 0);
        if (ret > 0) {
            for (int i = 0; i < 10; ++i) {
                if (fds[i].fd == serv_fd && fds[i].revents == POLLIN) {
                    // 客户端连接
                    int cfd = accept(fds[i].fd, NULL, NULL);
                    if (cfd < 0) {
                        continue;
                    }

                    int flags = fcntl(cfd, F_GETFL);
                    flags |= O_NONBLOCK;
                    fcntl(cfd, F_SETFL, flags);


                    for (int j = 0; j < 10; ++j) {
                        if (fds[j].fd <= 0) {
                            fds[j].fd = cfd;
                            fds[j].events = POLLIN;
                            break;
                        }
                    }
                } else if (fds[i].revents == POLLIN) {// 读
                    memset(buffer, 0, 512);
                    ret = read(fds[i].fd, buffer, 512);
                    if (ret < 0) {
                        printf("read errno: %d,errmsg: %s\n", errno, strerror(errno));
                        continue;
                    }

                    printf("client data: %s\n", buffer);

                    fds[i].revents = POLLOUT;
                } else if (fds[i].revents == POLLOUT) {// 写
                    char *data = "hello client\n";
                    int len = strlen(data);
                    ret = write(fds[i].fd, data, len);

                    fds[i].events = POLLIN;

                    if (ret < len) {
                        close(fds[i].fd);
                        fds[i].fd = -1;
                        fds[i].events = -1;
                    }
                } else if (fds[i].revents == POLLERR) {
                    // TODO
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    fds[i].events = -1;
                }
            }
        }
    }


    close(serv_fd);
}