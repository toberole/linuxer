//
// Created by linux on 19-1-19.
//


#include "xhttpd_engine.h"

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>

void send_status(int status, char *desc) {
    printf("HTTP/1.1 %d %s\r\n", status, desc);
}

void send_header(char *header, char *header_value) {
    printf("%s:%s\r\n", header, header_value);
}

void send_msg(char *msg, char *title, int flag) {
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

    exit(0);
}

void send_file(char *filename) {
    struct stat statbuff;
    int ret = stat("test.jpg", &statbuff);
    if (-1 == ret) {
        send_msg("file not found exception", "open error", -1);
        exit(0);
    }


    send_status(200, "ok");
    send_header("Content-Type", "image/jpeg");

    // 获取文件长度
    int filelen = statbuff.st_size;
    printf("Content-Length: %lld\r\n", filelen);
    send_header("Connection", "close");

    printf("\r\n");

    FILE *fp = fopen("test.jpg", "r");
    if (NULL == fp) {
        send_msg("file not found exception", "open error", -1);
        exit(0);
    }

    int ich = 0;// 注意ich 必须是int
    while ((ich = getc(fp)) != EOF) {
        putchar(ich);
    }

    fflush(stdout);
    fclose(fp);

    return;
}