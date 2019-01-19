//
// Created by linux on 19-1-19.
//


#include "xhttpd_engine.h"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

void send_status(int status, char *desc) {
    printf("HTTP/1.1 %d %s\r\n", status, desc);
}

void send_header(char *header, char *header_value) {
    printf("%s:%s\r\n", header, header_value);
}

int send_msg(char *msg, char *title, int flag) {
    if (flag < 0) {
        send_status(500, "error");
        send_header("Content-Type", "text/html; charset=utf-8");
        printf("\r\n");

        if (msg == NULL) msg = "error 500";

        printf("<html>\n"
               "<head><title>%d %s</title></head>\n"
               "<body bgcolor=\"#33cc99cc\">\n"
               "<br/>\n"
               "%s\n"
               "</body>\n"
               "</html>", 500, title, msg);

    } else {
        send_status(200, "OK");
        send_header("Content-Type", "text/html; charset=utf-8");
        printf("\r\n");

        if (msg == NULL) msg = "error 500";

        printf("<html>\n"
               "<head><title>%d %s</title></head>\n"
               "<body bgcolor=\"#33cc99cc\">\n"
               "<br/>\n"
               "%s\n"
               "</body>\n"
               "</html>", 500, title, msg);
    }

    fflush(stdout);

    return 0;
}

int send_file(char *filename) {
    int fd = open("test.jpg", O_WRONLY);
    if (fd < 0) {
        send_msg("open file error", "open error", -1);
    } else {
        send_status(200, "OK");
        send_header("Content-Type", "image/jpeg");
        // 获取文件长度
        int filelen = lseek(fd, 0L, SEEK_END);
        lseek(fd, 0L, SEEK_SET);

        char str[10];
        sprintf(str, "%d", filelen);

        send_header("Content-Length", str);

        printf("\r\n");

        char *data = (char *) malloc(filelen + 1);
        memset(data, 0, filelen + 1);
        read(fd, data, sizeof(data));

        printf("%s", "hello");
    }

    return 0;
}