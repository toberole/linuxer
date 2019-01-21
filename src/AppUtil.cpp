//
// Created by Apple on 2019/1/9.
//

#include "AppUtil.h"

#include <iconv.h>
#include <ctype.h>
#include <string.h>


bool isBlankChar(char ch) {
    return (ch == '\n' || ch == '\t' || ch == '\r' || ch == ' ');
}

void trim(char *str, int len) {
    char *start, *end;
    //去掉两端的空格
    start = str;            //指向首字符
    end = str + len - 1;        //指向最后一个字符
    while (*start && isBlankChar(*start))
        start++;        //如果是空格，首地址往前移一位，如果不是，则跳过该循环
    while (*end && isBlankChar(*end))
        *end-- = 0;        //如果是空格，末地址往前移一位，并赋结束符
    strcpy(str, start);       //把首地址还给str
}

int code_convert(
        char *from_charset, char *to_charset,
        char *inbuf, size_t inlen,
        char *outbuf, size_t outlen) {
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;

    cd = iconv_open(to_charset, from_charset);
    if (cd == 0)
        return -1;
    memset(outbuf, 0, outlen);
    if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
        return -1;
    iconv_close(cd);
    *pout = NULL;

    return 0;
}

int utf8s2gbks(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    return code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, outlen);
}

int gbks2utf8s(char *inbuf, size_t inlen, char *outbuf, size_t outlen) {
    return code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, outlen);
}


