// Цель: Эксперимент синхронизации через POSIX‑семафоры между потоками одного процесса.
//       Замена процессов на потоки для задачи 5.1 и измерение времени передачи MSG_COUNT сообщений.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>

#define MSG_COUNT 100000

typedef struct {
    sem_t empty;
    sem_t full;
    volatile char buffer;
} SharedData;

SharedData shm;

long diff_us(struct timeval s, struct timeval e) {
    return (e.tv_sec - s.tv_sec) * 1000000L + (e.tv_usec - s.tv_usec);
}

void* producer(void* arg) {
    struct timeval *start = arg;
    gettimeofday(start, NULL);
    for (int i = 0; i < MSG_COUNT; ++i) {
        sem_wait(&shm.empty);
        shm.buffer = (char)i;
        sem_post(&shm.full);
    }
    return NULL;
}

void* consumer(void* arg) {
    for (int i = 0; i < MSG_COUNT; ++i) {
        sem_wait(&shm.full);
        sem_post(&shm.empty);
    }
    struct timeval *end = arg;
    gettimeofday(end, NULL);
    return NULL;
}

int main() {
    pthread_t prod, cons;
    struct timeval start, end;

    sem_init(&shm.empty, 0, 1);
    sem_init(&shm.full,  0, 0);

    pthread_create(&prod, NULL, producer, &start);
    pthread_create(&cons, NULL, consumer, &end);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    long us = diff_us(start, end);
    double sec = us / 1e6;
    printf("Threads semaphore sync: %ld us, %.2f msgs/sec\n", us, MSG_COUNT / sec);

    sem_destroy(&shm.empty);
    sem_destroy(&shm.full);
    return 0;
}
