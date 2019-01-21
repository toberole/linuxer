//
// Created by Apple on 2019/1/9.
//

#ifndef LINUXER_UTIL_H
#define LINUXER_UTIL_H

#include <stdlib.h>

bool isBlankChar(char ch);

void trim(char *str, int len);

int utf8s2gbks(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

int gbks2utf8s(char *inbuf, size_t inlen, char *outbuf, size_t outlen);

int code_convert(char *from_charset, char *to_charset, char *inbuf, size_t inlen,
                 char *outbuf, size_t outlen);

#endif //LINUXER_UTIL_H
