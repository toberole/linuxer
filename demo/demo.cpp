#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void demo_test() {
    printf("demo_test\n");
}

void demo_test1(){
    // std::thread th(demo_test);
    // th.join();
}

void demo_test2(int fd){
    int fd1 = open("test.txt",O_CREAT);
    if (fd1<0){
        printf("fd faild\n");
        return;
    }
    char *data = "hello world";
    write(fd1,data, sizeof(data));
    close(fd1);
    // int flags = fcntl(fd,F_GETFD);
}

void test_fork(){

    char* data = "test fork";

    int k = 0;

    pid_t pid;

    //////////////////////////////////////////
    pid = fork();//创建进程 子进程从此代码之后开始运行
    if (pid == 0 ){
        // 当前是在子进程运行
        k++;
        printf("子进程运行 k = %d \n",k);
    }else if(pid>0){
        // 当前是在父进程运行
        k--;
        printf("父进程运行 k = %d \n",k);
    } else{
        perror("fork error");
    }

}

int main() {

    test_fork();


    printf("press any key to exit......\n");
    getchar();
    return 0;
}