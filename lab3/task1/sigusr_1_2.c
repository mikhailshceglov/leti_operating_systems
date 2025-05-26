
// Цель: Расширить назначение 1.1, установив пользовательский обработчик SIGUSR1
// в рабочем потоке, который регистрирует время получения и вызывает pthread_exit(NULL)
// для завершения только рабочего потока. Основной поток использует setitimer
// для отправки SIGUSR1 через 2 секунды, а затем присоединяется к рабочему потоку.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

static pthread_t t2;
static struct timespec start_time;

void print_rel_time(const char* prefix) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long sec  = now.tv_sec  - start_time.tv_sec;
    long nsec = now.tv_nsec - start_time.tv_nsec;
    if (nsec < 0) { sec--; nsec += 1000000000L; }
    printf("[%2lds.%09lds] %s\n", sec, nsec, prefix);
}

// Обработчик SIGUSR1 во втором потоке
void sigusr1_handler(int signo) {
    print_rel_time("Handler: received SIGUSR1, exiting thread");
    pthread_exit(NULL);
}

void* second_thread_func(void* arg) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

    signal(SIGUSR1, sigusr1_handler);
    int count = 0;
    while (1) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Second thread working cycle %d", ++count);
        print_rel_time(buf);
        usleep(500000);
    }
    return NULL;
}

void alarm_handler(int signo) {
    print_rel_time("Main: timer expired, sending SIGUSR1");
    pthread_kill(t2, SIGUSR1);
}

int main() {
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (pthread_create(&t2, NULL, second_thread_func, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    struct itimerval timer = {
        .it_value = { .tv_sec = 2, .tv_usec = 0 },
        .it_interval = {0, 0}
    };
    signal(SIGALRM, alarm_handler);
    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        exit(1);
    }

    pthread_join(t2, NULL);
    print_rel_time("Main: second thread exited, program ending");
    return 0;
}
