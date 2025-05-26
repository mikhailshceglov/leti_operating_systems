// Цель: Эксперимент синхронизации через pthread_mutex + pthread_cond между потоками одного процесса.
//       Замена процессов на потоки для задачи 5.1 и измерение времени передачи MSG_COUNT сообщений.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MSG_COUNT 100000

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    volatile int ready;
    volatile char buffer;
} SharedData;

SharedData shm;

long diff_us(struct timeval s, struct timeval e) {
    return (e.tv_sec - s.tv_sec) * 1000000L + (e.tv_usec - s.tv_usec);
}

void* producer_m(void* arg) {
    struct timeval *start = arg;
    gettimeofday(start, NULL);
    for (int i = 0; i < MSG_COUNT; ++i) {
        pthread_mutex_lock(&shm.mutex);
        shm.buffer = (char)i;
        shm.ready = 1;
        pthread_cond_signal(&shm.cond);
        while (shm.ready)
            pthread_cond_wait(&shm.cond, &shm.mutex);
        pthread_mutex_unlock(&shm.mutex);
    }
    return NULL;
}

void* consumer_m(void* arg) {
    struct timeval *end = arg;
    for (int i = 0; i < MSG_COUNT; ++i) {
        pthread_mutex_lock(&shm.mutex);
        while (!shm.ready)
            pthread_cond_wait(&shm.cond, &shm.mutex);
        shm.ready = 0;
        pthread_cond_signal(&shm.cond);
        pthread_mutex_unlock(&shm.mutex);
    }
    gettimeofday(end, NULL);
    return NULL;
}

int main() {
    pthread_t prod, cons;
    struct timeval start, end;

    pthread_mutex_init(&shm.mutex, NULL);
    pthread_cond_init(&shm.cond, NULL);
    shm.ready = 0;

    pthread_create(&prod, NULL, producer_m, &start);
    pthread_create(&cons, NULL, consumer_m, &end);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    long us = diff_us(start, end);
    double sec = us / 1e6;
    printf("Threads mutex+cond sync: %ld us, %.2f msgs/sec\n", us, MSG_COUNT / sec);

    pthread_mutex_destroy(&shm.mutex);
    pthread_cond_destroy(&shm.cond);
    return 0;
}
