/*
 * Программа запускает два процесса с одинаковым "приоритетом", но разной политикой планирования: SCHED_RR и SCHED_OTHER.
 * Каждый процесс выполняет цикл с итерациями и вызывает sched_yield() для наглядного сравнения поведения.
 * Используется sched_setscheduler(), usleep() и системные вызовы для отображения идентификаторов.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/syscall.h>
#include <string.h>

#define ITERATIONS 30

void run_rr_process() {
    struct sched_param param;
    param.sched_priority = 50;
    if (sched_setscheduler(0, SCHED_RR, &param) < 0) {
        perror("sched_setscheduler (SCHED_RR)");
        exit(EXIT_FAILURE);
    }
    printf("Процесс с SCHED_RR (PID=%d, TID=%ld) запущен с приоритетом %d\n", 
           getpid(), syscall(SYS_gettid), param.sched_priority);
    for (int i = 0; i < ITERATIONS; i++) {
        printf("SCHED_RR процесс (PID=%d): итерация %d\n", getpid(), i);
        fflush(stdout);
        sched_yield();
        usleep(100000);
    }
    exit(0);
}

void run_other_process() {
    printf("Процесс с SCHED_OTHER (PID=%d, TID=%ld) запущен с nice=0 (SCHED_OTHER по умолчанию)\n", 
           getpid(), syscall(SYS_gettid));
    for (int i = 0; i < ITERATIONS; i++) {
        printf("SCHED_OTHER процесс (PID=%d): итерация %d\n", getpid(), i);
        fflush(stdout);
        sched_yield();
        usleep(100000);
    }
    exit(0);
}

int main(int argc, char *argv[]) {
    pid_t pid_rr, pid_other;

    pid_rr = fork();
    if (pid_rr == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_rr == 0) {
        run_rr_process();
    }

    pid_other = fork();
    if (pid_other == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid_other == 0) {
        run_other_process();
    }

    waitpid(pid_rr, NULL, 0);
    waitpid(pid_other, NULL, 0);

    printf("Родитель: оба процесса завершены.\n");
    return 0;
}
