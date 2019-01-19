//
// Created by linux on 19-1-18.
//
#include <stdio.h>
#include <stdlib.h>


#include <string>

//中文字符
void test_demo01(){
    std::wstring wstr = L"中文字符";
    printf("len: %ld\n",wstr.size());

    std::wstring wstr1(wstr.c_str());
    printf("%s",wstr1);
}


#ifdef DEMO01

int main(int argc,char ** args){

    test_demo01();



    printf("press any key to exit");
    getchar();
    return 0;
}

#endif