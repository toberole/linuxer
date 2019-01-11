//
// Created by Apple on 2019/1/9.
//

#include "AppUtil.h"

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


