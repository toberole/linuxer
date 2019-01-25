//
// Created by linux on 19-1-18.
//
#include <stdio.h>
#include <stdlib.h>


#include <string>

//中文字符
void test_demo01() {
    std::wstring wstr = L"中文字符";
    printf("len: %ld\n", wstr.size());

    std::wstring wstr1(wstr.c_str());
    printf("%s", wstr1);
}

// typedef一个Func的函数指针
typedef void (*Func)();


void test_demo02() {
    printf("test_demo02\n");
}

int test_demo03(int a) {
    printf("test_demo03 a = %d\n", a);
}

#ifdef DEMO01

int main(int argc,char ** args){

    // test_demo01();

    Func f = test_demo02;
    f();

    printf("press any key to exit");
    getchar();
    return 0;
}

#endif