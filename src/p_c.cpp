//
// Created by linux on 19-1-17.
//

#include "p_c.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
// 共享资源
int count = 0;
pthread_mutex_t mutex;
pthread_cond_t cond;

void p() {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count != 0) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 生产
        count++;
        printf("生产了\n\n");

        // 通知消费者消费
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
}


// 消费者
void c() {
    while (1) {
        pthread_mutex_lock(&mutex);
        while (count != 1) {
            pthread_cond_wait(&cond, &mutex);
        }

        // 生产
        count--;
        printf("消费了\n\n");

        // 通知生产者生产
        pthread_cond_signal(&cond);

        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
}

void test_p_c() {

    int ret = pthread_mutex_init(&mutex, NULL);

    if (ret != 0) {
        printf("pthread_mutex_init error\n");
    }

    ret = pthread_cond_init(&cond, NULL);
    if (ret != 0) {
        printf("pthread_cond_init error\n");
    }

    pthread_t ph1;
    pthread_t ph2;

    ret = pthread_create(&ph1, NULL, (void *(*)(void *)) p, NULL);
    if (ret < 0) {
        printf("pthread_create ph1 error\n");
        return;
    }

    ret = pthread_create(&ph2, NULL, (void *(*)(void *)) c, NULL);
    if (ret < 0) {
        printf("pthread_create ph2 error\n");
        return;
    }

    pthread_join(ph1, NULL);
    pthread_join(ph2, NULL);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}