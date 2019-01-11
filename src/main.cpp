#include <iostream>
#include <thread>
#include <mutex>
#include <string>

#include <cstring>

#include "AppUtil.h"
#include "Log.h"
#include "Book.h"

#include "Client.h"
#include "Server.h"

#include "Select.h"


void test() {
    std::cout << "--- test ---" << std::endl;
}

void test0() {
    std::thread th1(test);
    std::thread th2(test);

    th1.join();
    th2.join();
}

void test1(std::mutex &mtx, int &count) {

    while (count < 100) {
        std::lock_guard<std::mutex> m(mtx);
        if (count < 100) {
            std::cout << "thread_ID: " << std::this_thread::get_id() << " count = " << count << "\n";
            count++;
            std::this_thread::sleep_for(std::chrono::microseconds(200));
        }
    }
}

void test_1() {
    std::mutex mtx;
    int count = 0;
    std::thread *ths[10];
    for (int i = 0; i < 5; i++) {
        ths[i] = new std::thread(test1, std::ref(mtx), std::ref(count));
    }

    for (auto th:ths) {
        (*th).join();
    }


    for (auto th:ths) {
        delete th;
    }

}

/**
 *
 * @param argc 命令行参数个数
 *
 * @param argv 命令行参数数组
 * argv[0] 表示程序的名字
 *
 */
int main(int argc, char const *argv[]) {
    std::cout << "hello main 计算机" << std::endl;

    if (argc > 1) {
        const char *cmd = argv[1];
        char *cmd_str = new char[strlen(cmd) + 1]{0};
        strcpy(cmd_str, cmd);
        trim(cmd_str, strlen(cmd_str));
        printf("cmd_str: %s\n", cmd_str);

        if (strcmp(cmd_str, "tcps") == 0) {
            startTCPServer();
        } else if (strcmp(cmd_str, "tcpc") == 0 && argc > 2) {
            char *argv2 = new char[strlen(argv[2]) + 1]{0};
            strcpy(argv2, argv[2]);
            trim(argv2, strlen(argv2));
            startTCPClient(argv2);
        } else if (strcmp(cmd_str, "udps") == 0) {
            startUDPServer();
        } else if (strcmp(cmd_str, "udpc") == 0 && argc > 2) {
            char *argv2 = new char[strlen(argv[2]) + 1]{0};
            strcpy(argv2, argv[2]);
            trim(argv2, strlen(argv2));
            printf("main udpc ip: %s\n", argv2);
            startUDPClient(argv2);
        }
    }

    // startSelectTCPServer();
    // test_select();

    startSelectTCPServerN2N();

    std::cout << "press any key to exit ......" << std::endl;
    getchar();
    return 0;
}
