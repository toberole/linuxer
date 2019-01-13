#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

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

#ifdef DEMO_FILE

int main() {

    // test_fork();
    // test_exec();

    // test_orphan();
    // test_zombie();
    test_wait();

    printf("press any key to exit......\n");
    // getchar();
    return 0;
}

#endif