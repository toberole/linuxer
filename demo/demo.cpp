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
#include <signal.h>
#include <sys/time.h>
#include <pthread.h>

#include "sync_.h"

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
        close(rwfd[1]);//只做读操作,子进程关闭写fd，不用最好是关闭
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
    if (pipe(fds) == -1) {
        printf("create pipe error\n");
        return;
    }
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

// 线程之间通信 pipe
void opt_pipe(int pipe_fd, int mode) {
    if (mode == 0) {
        // 读取
        char buffer[200] = {0};
        read(pipe_fd, buffer, 200);
        printf("线程读取到的数据： %s\n", buffer);

        close(pipe_fd);
    } else if (mode == 1) {
        // 写
        char *data = "hello pipe reader";
        write(pipe_fd, data, strlen(data));

        close(pipe_fd);
    }
}

void test_pipe_opt() {
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        printf("create pipe error\n");
        return;
    }

    std::thread reader(opt_pipe, pipe_fds[0], 0);
    std::thread writer(opt_pipe, pipe_fds[1], 1);

    reader.join();
    writer.join();
}


// mmap 映射文件
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

// 进程间通信
void test_mmap1() {
    int fd = open("./test.mmap", O_RDWR);
    if (fd == -1) {
        printf("open file error\n");
        return;
    }
    // 获取文件长度
    int len = lseek(fd, 0, SEEK_END);

    // 后续就可以操作该指针的指向的内存缓存区
    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        printf("mmap error\n");
        return;
    }

    char buffer[5] = {'A', 'A', 'A', 'A', 'A'};
    pid_t pid = fork();
    if (pid == 0) {
        strcpy((char *) ptr, buffer);
    } else {
        sleep(2);
        printf("data: %s\n", (char *) ptr);

        // 关闭文件映射区
        munmap(ptr, len);

        // 回收子进程
        while (wait(NULL) != -1) {

        }
    }
}

// 创建匿名映射区
void test_mmap2() {
    int len = 100;
    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON/*匿名映射区必须该参数*/, -1/*不需要文件描述符*/, 0);

    strcpy((char *) ptr, "hello ANNO MMAP");

    printf("ANNO MMAP DATA: %s\n", (char *) ptr);

    // 关闭文件映射区
    munmap(ptr, len);
}

// 创建映射区 非血缘关系进程通信
// 不同的进程映射同一个磁盘文件，注意偏移量需要一样
void test_mmap3() {
    int fd = open("./anno.txt", O_CREAT | O_RDWR, 0664);
    if (fd < 0) {
        printf("open file error\n");
        return;
    }

    int len = 4096;

    // 空文件需要文件拓展
    ftruncate(fd, len);//lseek(fd,4096,SEEK_END);
    // 文件获取文件长度，不要直接使用len，应为拓展不一定就是len
    len = lseek(fd, 0, SEEK_END);

    void *ptr = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    strcpy((char *) ptr, "hello ANNO MMAP");

    printf("ANNO MMAP DATA: %s\n", (char *) ptr);

    // 关闭文件映射区
    munmap(ptr, len);
}

// 信号
void test_signal() {
    sigset_t set;
    // 获取未决信号集
    int ret = sigpending(&set);
    if (ret == -1) {
        printf("获取未决信号集失败\n");
    }

    // 屏蔽信号
    // 1、自定义一个信号集
    sigset_t self_set;
    // 清空集合
    sigemptyset(&self_set);
    // 2 添加需要阻塞的信号
    sigaddset(&self_set, SIGQUIT);
    // 设置自定义信号集给内核信号集
    sigset_t oldset;// 接受设置之前的阻塞信号集
    sigprocmask(SIG_BLOCK, &self_set, &oldset);

    // 判断前31号信号
    for (int i = 0; i < 31; ++i) {
        int res = sigismember(&set, i);
        if (res == 1) {
            printf("决信号: %d\n", i);
        }
    }
}

void test_sigaction_XXX(int sig) {

}

void test_sigaction() {
    struct sigaction act;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    // 添加临时屏蔽的信号
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_handler = test_sigaction_XXX;
    sigaction(SIG_BLOCK, &act, NULL);

}

// 创建守护进程
void test_daemon() {
    pid_t pid = fork();
    if (pid > 0) {
        // 父进程死亡
        raise(SIGKILL);
    } else {
        // 操作子进程变成守护进程
        setsid();// 创建新会话
        sleep(1);
    }
};

void do_work(int sig) {
    int fd = open("temp.txt", O_CREAT | O_RDWR | O_APPEND, 0664);

    time_t current_time;
    // 获取当前时间
    time(&current_time);
    // 格式化时间
    char *timeStr = ctime(&current_time);

    if (timeStr) {
        write(fd, timeStr, strlen(timeStr));
        write(fd, "\r\n", strlen("\r\n"));
        close(fd);
    }
}

/**
 * 创建守护进程
 * 每隔2s获取一次系统时间 写到文件
 */
void test_daemon1() {
    // 创建守护进程
    pid_t pid = fork();
    if (pid > 0) {
        raise(SIGKILL);
    } else if (pid == 0) {
        // 变成会长 脱离会话终端 变成守护进程
        setsid();

        // 改变当前工作目录
        //chdir("/"); // 注意需要有改变到目标位置的权限

        // 重设文件掩码
        umask(0);// 相当于chmod 777

        // 关闭文件描述符
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

        // 具体业务
        int which = ITIMER_REAL;// 对应捕获SIGALRM信号,定时法则不同 对应的信号也不同
        struct itimerval new_value;
        // 间隔时间
        new_value.it_interval.tv_sec = 2; // 秒
        new_value.it_interval.tv_usec = 0;// 微秒
        // 第一次开始的时间
        new_value.it_value.tv_sec = 2;
        new_value.it_value.tv_usec = 0;

        int sig = SIGALRM;
        struct sigaction act;
        act.sa_flags = 0;
        act.sa_handler = do_work;
        // 每次执行任务的时候不需要屏蔽信号
        sigemptyset(&act.sa_mask);
        sigaction(sig, &act, NULL);

        // 设置定时器
        setitimer(which, &new_value, NULL);

        // 不允许退出
        while (1);
    }
};


int n = 100;

void test_p(void *arg) {
    // 打印线程id
    printf("线程id: %d\n", pthread_self());
    pthread_exit((void *) &n);
}

void test_pthread() {
    pthread_t pth_id;
    int ret = pthread_create(&pth_id, NULL, (void *(*)(void *)) test_p, NULL);
    if (ret != 0) {
        printf("pthread_create error %s\n", strerror(ret)/* 线程里面必须使用strerror打印error信息 */);
        return;
    }

    void *__thread_return;
    pthread_join(pth_id, &__thread_return/* 读取线程退出时携带的信息 */);


    printf("子线程退出时候的信息： %d\n", *(int *) __thread_return);
}

// 通过属性设置线程分离
void test_pthread1() {
    pthread_t pth_id;
    pthread_attr_t attr;

    int s = pthread_attr_init(&attr);
    if (s != 0) {
        printf("pthread_attr_init error\n");
    }

    // 设置分离属性
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);// PTHREAD_CREATE_JOINABLE

    int ret = pthread_create(&pth_id, &attr, (void *(*)(void *)) test_p, NULL);
    if (ret != 0) {
        printf("pthread_create error %s\n", strerror(ret)/* 线程里面必须使用strerror打印error信息 */);
        return;
    }

    s = pthread_attr_destroy(&attr);
    if (s != 0) {
        printf("pthread_attr_destroy error\n");
    }

    printf("父子进程分离\n");
}


#ifdef DEMO_FILE

int main() {

    // test_fork();
    // test_exec();

    // test_orphan();
    // test_zombie();
    // test_wait();

    // test_pipe();
    // test_mmap();
    // test_pipe_opt();
    // test_mmap1();
    // test_mmap2();
    // test_mmap3();

//    int n = 0;
//    alarm(1);
//    while(1){
//        printf("n = %d \n",n++);
//    }


    // test_daemon1();
    // test_pthread1();

    // test_mutex_sync();

//    std::thread ths[5];
//    for (int i = 0; i < 5; ++i) {
//        ths[i] = std::thread(tt);;
//    }
//
//    for (int j = 0; j < 5; ++j) {
//        ths[j].join();
//    }






    printf("press any key to exit......\n");
    getchar();
    return 0;
}

#endif