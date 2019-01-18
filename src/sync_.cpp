//
// Created by linux on 19-1-16.
//

#include "sync_.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct data_ {
    pthread_mutex_t mutex_sync;
    pthread_rwlock_t rw_lock;
    sem_t sem;
    int count;
};

////////////////// 互斥锁 //////////////////////
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

/////////////////////// 读写锁 /////////////////////
void fun3(void *arg) {
    struct data_ *p = (struct data_ *) arg;

    pthread_rwlock_rdlock(&((*p).rw_lock));

    printf("获取读锁\n");

    pthread_rwlock_unlock(&((*p).rw_lock));

}

void fun4(void *arg) {
    struct data_ *p = (struct data_ *) arg;

    pthread_rwlock_wrlock(&((*p).rw_lock));

    printf("获取写锁\n");

    pthread_rwlock_unlock(&((*p).rw_lock));
}

///////////////////// 信号量 ////////////////////
void fun5(void *arg) {
    printf("---- fun5 ----\n");

    struct data_ *p = (struct data_ *) arg;
    sem_wait(&((*p).sem));// pv操作 --
    ((*p).count)++;
    sem_post(&((*p).sem));// // pv操作 ++
}

void fun6(void *arg) {
    printf("---- fun6 ----\n");

    struct data_ *p = (struct data_ *) arg;
    sem_wait(&((*p).sem));// pv操作 --
    ((*p).count)++;
    sem_post(&((*p).sem));// // pv操作 ++
}


void test_mutex_sync() {
    struct data_ d;
    d.count = 0;

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

void test_rwlock_sync() {
    struct data_ d;
    d.count = 0;


    int ret = pthread_rwlock_init(&(d.rw_lock), NULL);

    if (ret != 0) {
        printf("pthread_mutex_init error\n");
        return;
    }

    pthread_t ph1;
    pthread_t ph2;

    ret = pthread_create(&ph1, NULL, (void *(*)(void *)) fun3, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph1 error\n");
        return;
    }

    ret = pthread_create(&ph2, NULL, (void *(*)(void *)) fun4, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph2 error\n");
        return;
    }

    pthread_join(ph1, NULL);
    pthread_join(ph2, NULL);

    pthread_rwlock_destroy(&(d.rw_lock));
}

void test_sem_sync() {
    struct data_ d;
    d.count = 0;

    int value = 1;
    int ret = sem_init(&d.sem, 0/*标记是线程同步 还是进程同步 1: 进程同步 0: 线程同步*/, value/* 初始资源个数 */ );

    if (ret != 0) {
        printf("sem_init error\n");
        return;
    }

    pthread_t ph1;
    pthread_t ph2;

    ret = pthread_create(&ph1, NULL, (void *(*)(void *)) fun5, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph1 error\n");
        return;
    }

    ret = pthread_create(&ph2, NULL, (void *(*)(void *)) fun6, (void *) &d);
    if (ret < 0) {
        printf("pthread_create ph2 error\n");
        return;
    }

    pthread_join(ph1, NULL);
    pthread_join(ph2, NULL);

    printf("count: %d\n", d.count);

    sem_destroy(&d.sem);
}



