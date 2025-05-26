/*
 * Программа демонстрирует настройку политик планирования и приоритетов для POSIX-потоков.
 * Вначале все потоки работают по умолчанию (SCHED_OTHER), затем в эксперименте 5.6.1 все переводятся на SCHED_RR с приоритетом 50,
 * а в эксперименте 5.6.2 один поток получает приоритет выше — 60. Используется sched_yield() и задержки для наблюдения за порядком выполнения.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

#define NUM_THREADS 3
#define ITERATIONS 30
#define DELAY_USEC 100000

void print_thread_sched(pthread_t thread, const char *msg) {
    int policy;
    struct sched_param param;
    if (pthread_getschedparam(thread, &policy, &param) != 0) {
        perror("pthread_getschedparam");
        return;
    }
    const char *policy_str;
    switch(policy) {
        case SCHED_OTHER: policy_str = "SCHED_OTHER"; break;
        case SCHED_FIFO:  policy_str = "SCHED_FIFO"; break;
        case SCHED_RR:    policy_str = "SCHED_RR"; break;
        default:          policy_str = "UNKNOWN"; break;
    }
    printf("%s: ТID=%lu, политика=%s, приоритет=%d\n", msg,
           (unsigned long) pthread_self(), policy_str, param.sched_priority);
}

void* thread_func(void* arg) {
    int thread_num = *(int*)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        printf("Поток #%d (TID=%lu): итерация %d\n", thread_num,
               (unsigned long) pthread_self(), i);
        fflush(stdout);
        sched_yield();
        usleep(DELAY_USEC);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];
    int i;
    int ret;

    for (i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i + 1;
        ret = pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]);
        if (ret != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(ret));
            exit(EXIT_FAILURE);
        }
    }

    printf("=== Начальные параметры (все потоки) ===\n");
    print_thread_sched(pthread_self(), "Главный поток");
    for (i = 0; i < NUM_THREADS; i++) {
        print_thread_sched(threads[i], "Дочерний поток");
    }
    printf("========================================\n");

    sleep(2);

    printf("\n*** Эксперимент 5.6.1: Изменяем для ВСЕХ потоков на SCHED_RR с приоритетом 50 ***\n");
    struct sched_param param_all;
    param_all.sched_priority = 50;
    if (pthread_setschedparam(pthread_self(), SCHED_RR, &param_all) != 0) {
        perror("pthread_setschedparam (главный)");
    }
    for (i = 0; i < NUM_THREADS; i++) {
        if (pthread_setschedparam(threads[i], SCHED_RR, &param_all) != 0) {
            perror("pthread_setschedparam (дочерний)");
        }
    }

    printf("=== Параметры после смены политики для всех потоков ===\n");
    print_thread_sched(pthread_self(), "Главный поток");
    for (i = 0; i < NUM_THREADS; i++) {
        print_thread_sched(threads[i], "Дочерний поток");
    }
    printf("=======================================================\n");

    sleep(5);

    printf("\n*** Эксперимент 5.6.2: Изменяем параметры только для одного потока ***\n");
    struct sched_param param_one;
    param_one.sched_priority = 60;
    if (pthread_setschedparam(threads[1], SCHED_RR, &param_one) != 0) {
        perror("pthread_setschedparam для одного потока");
    }

    printf("=== Параметры после изменения только для потока 2 ===\n");
    print_thread_sched(threads[1], "Дочерний поток 2");
    for (i = 0; i < NUM_THREADS; i++) {
        if (i != 1) {
            print_thread_sched(threads[i], "Другой дочерний поток");
        }
    }
    printf("=========================================================\n");

    sleep(5);

    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Главный поток: все потоки завершены.\n");
    return 0;
}
