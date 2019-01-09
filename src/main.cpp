#include <iostream>
#include <thread>
#include <mutex>
#include <string>


#include "Log.h"
#include "Book.h"

void test(){
    std::cout<<"--- test ---"<<std::endl;
}

void test0(){
    std::thread th1(test);
    std::thread th2(test);

    th1.join();
    th2.join();
}

void test1(std::mutex &mtx, int &count){
    
    while(count<100){
        std::lock_guard<std::mutex> m(mtx);
        if(count<100){
            std::cout<<"thread_ID: "<<std::this_thread::get_id()<<" count = "<<count<<"\n";
            count++;
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    }
}

void test_1(){
    std::mutex mtx;
    int count = 0;
    std::thread * ths[10];
    for(int i =0;i<5;i++){
        ths[i] = new std::thread(test1,std::ref(mtx),std::ref(count));
    }
    
    for(auto th:ths)
    {
       (*th).join();
    }

    
    for(auto th:ths)
    {
        delete th;
    }

}

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFAULT_PORT 8000
#define MAXLINE 4096


void startServer(){
    int socket_fd;
    int connect_fd;
    struct sockaddr_in serveradd;
    char buffer[1024*4];

    // 初始化socket
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if(-1 == socket_fd){
        printf("create socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    // 初始化serveradd
    memset(&serveradd,0,sizeof(sockaddr_in));
    serveradd.sin_family = AF_INET;
    // INADDR_ANY 系统自动获取本机地址
    serveradd.sin_addr.s_addr = htol(INADDR_ANY);
    // 设置端口号
    serveradd.sin_port = htos(DEFAULT_PORT);

    // bind 绑定套接字
    if(bind(socket_fd,(struct sockadd*)&serveradd,sizeof(serveradd))==-1){
        printf("bind socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);
    }

    // 监听
    if(listen(socket_fd,10) == -1){
        printf("listen socket error: %s(errno: %d)\n",strerror(errno),errno);
        exit(0);        
    }

    // 接受客户端的连接
    while(1){
        if((connect_fd = accept(socket_fd,(struct sockadd*)NULL,NULL))==-1){
            printf("accept socket error: %s(errno: %d)\n",strerror(errno),errno);
            continue;
        }
    }

}

int main(int argc, char const *argv[])
{
    std::cout<<"hello main"<<std::endl;



    
    

    
       
    std::cout<<"press any key to exit ......"<<std::endl;
    getchar();
    return 0;
}
