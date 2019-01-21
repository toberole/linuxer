//
// Created by linux on 19-1-19.
//


#include "xhttpd_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
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
    // /test.jpg
    if (NULL == filename) {
        send_msg("fileName == NULL exception", "request error", -1);
        exit(0);
    }

    int fileNameLen = -1;
    fileNameLen = strlen(filename);
    char *fn = (char *) malloc(fileNameLen - 1);
    memset(fn, 0, fileNameLen - 1);
    strcpy(fn, filename + 1);

    struct stat statbuff;
    int ret = stat(fn, &statbuff);
    if (-1 == ret) {
        send_msg("file not found exception", "open error", -1);
        delete fn;
        exit(0);
    }


    send_status(200, "ok");
    // 需要根据文件后缀 确定IMIE
    send_header("Content-Type", "image/jpeg");

    // 获取文件长度
    int filelen = statbuff.st_size;
    printf("Content-Length: %lld\r\n", filelen);
    send_header("Connection", "close");

    printf("\r\n");

    FILE *fp = fopen(fn, "r");
    if (NULL == fp) {
        send_msg("file not found exception", "open error", -1);
        delete fn;
        exit(0);
    }


//    int ich = 0;// 注意ich 必须是int
//    while ((ich = getc(fp)) != EOF) {
//        putchar(ich);
//    }

    // FILE *fp = fopen("/home/linux/web/dir/test.txt", "r");
//    unsigned char buffer[1024] = {0};
//    if (fp != NULL) {
//        while (!feof(fp)) {// 注意二进制文件读写的坑
//            fgets(buffer, sizeof(buffer) - 1, fp);
//            puts(buffer);
//            fflush(stdout);
//            memset(buffer, 0, 1024);
//        }
//    }

    unsigned char buffer[1024] = {0};
    int rc;
    while ((rc = fread(buffer, sizeof(unsigned char), 1024, fp)) != 0) {
        /**
         * size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
         * size: 单个元素的大小
         * nmemb：要读取元素的个数
         */
        fwrite(buffer, sizeof(unsigned char)/* 指单个元素的大小 */, rc, stdout);
    }


    delete fn;
    fflush(stdout);
    fclose(fp);

    exit(0);
}