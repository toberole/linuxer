//
// Created by linux on 19-1-17.
//

#ifndef LINUXER_P_C_H
#define LINUXER_P_C_H


#include <pthread.h>

// 共享资源
extern int count;
extern pthread_mutex_t mutex;
extern pthread_cond_t cond;

// 生产者
void p();

// 消费者
void c();

void test_p_c();

#endif //LINUXER_P_C_H
