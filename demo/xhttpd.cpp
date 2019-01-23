#include <stdio.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "xhttpd_engine.h"

/**
 * xinetd 接受到http请求会调用该程序
 *
 * xinetd在调用该程序的时候会通过args[1]传递配置文件里面的server_args参数值
 *
 * xinetd 调用该程序的时候 工作目录是该在程序所在的位置 如果需要到其他的位置：可以使用chdir改变工作目录
 *
 */

#define BUFFER_LEN 2048

int i = 0;

int main(int argc, char **argv) {
    if (argc < 2) {
        send_msg("argc < 2", NULL, -1);
    }

    int ret = -1;
    // 改变当前工作目录
    ret = chdir(argv[1]);// argv[1] xinetd 配置文件中配置的server_flags参数
    umask(0);
    if (ret < 0) {
        send_msg("chdir error", NULL, -1);
    }

    char line[BUFFER_LEN] = {0};
    char method[BUFFER_LEN] = {0};
    char path[BUFFER_LEN] = {0};
    char protocol[BUFFER_LEN] = {0};

    // 获取http请求头部 [GET /readme.txt HTTP/1.1]
    if (fgets(line, sizeof(line), stdin) == NULL) {// fgets 读取一行 \r\n终止
        send_msg("request error", NULL, -1);
    }

    if (sscanf(line, "%[^ ] %[^ ] %[^ ]", method, path, protocol) != 3) {
        send_msg("request error", NULL, -1);
    }

//        char *data = (char *) malloc(strlen(method) + strlen(path) + strlen(protocol));
//        // 字符串拼接的方式
//        sprintf(data, "%s %s %s", method, path, protocol);
//
//        send_msg(data, "send data", 0);
//
//
//        delete data;

//    while (fgets(line, sizeof(line), stdin) != NULL) {// fgets 读取一行 \r\n终止
//        if (strcmp(line, "\r\n") || strcmp(line, "\n")) {
//            break;
//        }
//    }




    send_file(path);

    return 0;
}












