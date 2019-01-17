//
// Created by linux on 19-1-16.
//
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "sync_.h"

struct data_ {
    pthread_mutex_t mutex_sync;
    int count;
};

void fun1(void *arg) {
    struct data_ *p = (struct data_ *) arg;

    while ((*p).count < 20) {
        pthread_mutex_lock(&((*p).mutex_sync));

        printf("fun1： %d\n", (*p).count);

        (*p).count++;

        pthread_mutex_unlock(&((*p).mutex_sync));

        sleep(1);
    }
}

void fun2(void *arg) {
    struct data_ *p = (struct data_ *) arg;
    while ((*p).count < 20) {
        pthread_mutex_lock(&((*p).mutex_sync));

        printf("fun2： %d\n", (*p).count);

        (*p).count++;

        pthread_mutex_unlock(&((*p).mutex_sync));

        sleep(1);
    }
}

void test_sync() {
    struct data_ d;

    int ret = pthread_mutex_init(&(d.mutex_sync), NULL);

    if (ret != 0) {
        printf("pthread_mutex_init error\n");
        return;
    }

    pthread_t ph1;
    pthread_t ph2;

    ret = pthread_create(&ph1, NULL, (void *(*)(void *)) fun1, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph1 error\n");
        return;
    }

    ret = pthread_create(&ph2, NULL, (void *(*)(void *)) fun2, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph2 error\n");
        return;
    }

    pthread_join(ph1, NULL);
    pthread_join(ph2, NULL);

    pthread_mutex_destroy(&(d.mutex_sync));
}

