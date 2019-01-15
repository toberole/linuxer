#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>


void demo_test() {
    printf("demo_test\n");
}

void demo_test1() {
    // std::thread th(demo_test);
    // th.join();
}

void demo_test2(int fd) {
    int fd1 = open("test.txt", O_CREAT | O_WRONLY);
    if (fd1 < 0) {
        printf("fd faild\n");
        return;
    }
    char *data = "hello world";
    write(fd1, data, sizeof(data));
    close(fd1);
    // int flags = fcntl(fd,F_GETFD);
}

void test_fork() {

    char *data = "test fork";

    int k = 0;

    pid_t pid;

    //////////////////////////////////////////
    pid = fork();//创建进程 子进程从此代码之后开始运行
    if (pid == 0) {
        // 当前是在子进程运行
        k++;
        printf("子进程运行 k = %d \n", k);
    } else if (pid > 0) {
        // 当前是在父进程运行
        k--;
        printf("父进程运行 k = %d \n", k);
    } else {
        perror("fork error");
    }

}

void test_exec() {
    pid_t pid = fork();

    if (pid == 0) {
        execl("/bin/ls", "ls"/*用来占位 没有实际意义*/, "-al" /* 给ls 程序的参数*/, NULL/*标记传递参数结束*/);
    } else if (pid > 0) {
        printf("父进程");
    }

    return;
}

// 测试孤儿进程
void test_orphan() {
    pid_t pid = fork();

    if (pid == 0) {
        sleep(1);
        printf("孤儿进程获取父进程pid： %d\n", getpid());
    } else if (pid > 0) {
        printf("父进程\n");
    }
}


// 测试僵尸进程
void test_zombie() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("僵尸进程获取父进程pid： %d\n", getpid());
    } else if (pid > 0) {
        sleep(10);
        printf("父进程\n");
    }
}

void test_wait() {
    pid_t pid = fork();

    if (pid == 0) {
        printf("子进程获取父进程pid： %d\n", getpid());

        sleep(10);
    } else if (pid > 0) {

        printf("父进程wait 回收子进程\n");
        int status;
        // 循环回收子进程
        while (wait(&status) > 0) {
            if (WIFEXITED(status)) {
                // 子进程正常死亡
                printf("子进程正常死亡\n");
            }
        }

        /*

         do {
                   w = waitpid(cpid, &wstatus, WUNTRACED | WCONTINUED);
                   if (w == -1) {
                       perror("waitpid");
                       exit(EXIT_FAILURE);
                   }

                   if (WIFEXITED(wstatus)) {
                       printf("exited, status=%d\n", WEXITSTATUS(wstatus));
                   } else if (WIFSIGNALED(wstatus)) {
                       printf("killed by signal %d\n", WTERMSIG(wstatus));
                   } else if (WIFSTOPPED(wstatus)) {
                       printf("stopped by signal %d\n", WSTOPSIG(wstatus));
                   } else if (WIFCONTINUED(wstatus)) {
                       printf("continued\n");
                   }
            } while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
         */

    }
}

// 匿名管道
void test_pipe() {
    int rwfd[2];
    int res = pipe(rwfd);
    if (res == -1) {
        printf("create pipe error: %s", strerror(errno));
    }

    printf("pipe rid: %d\n", rwfd[0]);
    printf("pipe wid: %d\n", rwfd[1]);

    pid_t pid = fork();
    if (pid == 0) {// 子进程
        char buffer[20] = {0};
        close(rwfd[1]);//子进程关闭写fd，只做读操作
        read(rwfd[0], buffer, 20);

        close(rwfd[0]);

        printf("父进程写的数据是： %s\n", buffer);

    } else if (pid > 0) {// 父进程
        char *buffer1 = "hello child";
        close(rwfd[0]);//子进程关闭写fd，只做写操作
        write(rwfd[1], buffer1, strlen(buffer1));
        printf("父进程写数据\n");

        sleep(3);

        close(rwfd[1]);

        while (wait(nullptr) != -1) {
            printf("父进程回收子进程\n");
        }
    }
}

/*
 *
设置文件描述符对应的属性
int flags = fcntl(fd,F_GETFL);
flags |= O_NONBLOCK;// 需要设置的属性
fcntl(fd,flags);

 */

// 修改pipe非阻塞
void test_pipe1() {
    int fds[2];
    pid_t pid = fork();

    fcntl(fds[0], F_GETFL);
    if (pid == 0) {
        close(fds[1]);

        int flags = fcntl(fds[0], F_GETFL);
        flags |= O_NONBLOCK;// 需要设置的属性
        fcntl(fds[0], F_SETFL, flags);

    } else if (pid > 0) {

    }
}

// mmap
void test_mmap() {
    int fd = open("./test.mmap", O_RDWR);
    if (fd == -1) {
        printf("open file error\n");
        return;
    }
    // 获取文件长度
    int len = lseek(fd, 0, SEEK_END);

    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        printf("mmap error\n");
        return;
    }

    // 打印文件内容
    printf("文件内容： %s\n", (char *) ptr);

    // 关闭文件映射区
    munmap(ptr, len);
}


#ifdef DEMO_FILE

int main() {

    // test_fork();
    // test_exec();

    // test_orphan();
    // test_zombie();
    // test_wait();

    // test_pipe();
    test_mmap();

    printf("press any key to exit......\n");
    getchar();
    return 0;
}

#endif