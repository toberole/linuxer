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
    char *fn = (char *) malloc(fileNameLen);
    memset(fn, 0, fileNameLen);
    strcpy(fn, filename + 1);

    // test.jpg
    // .jpg
    int fn_len = strlen(fn);
    int pos = -1;
    for (int i = 0; i < fn_len; ++i) {
        if (fn[i] == '.') {
            pos = i;
            break;
        }
    }

    char *suffix_buf = (char *) malloc(sizeof(char) * (fn_len - pos));
    if (-1 != pos) {
        strcpy(suffix_buf, fn + pos);
    }

    char *suffix = getContentType(suffix_buf);

    struct stat statbuff;
    int ret = stat(fn, &statbuff);
    if (-1 == ret) {
        send_msg("file not found exception", "open error", -1);
        free(fn);
        exit(0);
    }

    send_status(200, "ok");
    // 需要根据文件后缀 确定IMIE

    send_header("Content-Type", suffix);

    // 获取文件长度
    int filelen = statbuff.st_size;
    printf("Content-Length: %lld\r\n", filelen);
    send_header("Connection", "close");

    printf("\r\n");

    FILE *fp = fopen(fn, "r");
    if (NULL == fp) {
        send_msg("file not found exception", "open error", -1);
        free(fn);
        exit(0);
    }

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


    free(fn);
    free(suffix_buf);
    fflush(stdout);
    fclose(fp);

    exit(0);
}

char *getContentType(char *suffix) {
    if (strcmp(".jpg", suffix) == 0 ||
        strcmp(".jpeg", suffix) == 0 ||
        strcmp(".png", suffix) == 0) {
        return "image/jpeg";
    } else {
        return "text/html; charset=utf-8";
    }
}