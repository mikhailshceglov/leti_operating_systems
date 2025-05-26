/*
 * Программа запускает три дочерних процесса с политикой планирования SCHED_FIFO.
 * В режиме по умолчанию приоритеты равны, в режиме "diff" задаются разные приоритеты.
 * Каждый процесс выполняет цикл с вызовами sched_yield() и завершает выполнение с кодом, равным своему номеру.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

#define NUM_CHILD 3
#define ITERATIONS 30

void run_child(int child_num, int priority) {
    struct sched_param param;
    param.sched_priority = priority;
    if (sched_setscheduler(0, SCHED_FIFO, &param) < 0) {
        perror("sched_setscheduler");
        exit(EXIT_FAILURE);
    }
    printf("Дочерний процесс %d (PID=%d) запущен с SCHED_FIFO, приоритет %d\n", child_num, getpid(), priority);
    
    for (int i = 0; i < ITERATIONS; i++) {
        printf("Дочерний процесс %d (PID=%d): итерация %d\n", child_num, getpid(), i);
        fflush(stdout);
        sched_yield();
    }
    printf("Дочерний процесс %d (PID=%d) завершает работу.\n", child_num, getpid());
    exit(child_num);
}

int main(int argc, char* argv[]) {
    int mode_equal = 1;
    if (argc > 1) {
        if (strcmp(argv[1], "diff") == 0) {
            mode_equal = 0;
        }
    }

    pid_t pids[NUM_CHILD];
    int base_priority = 50;

    for (int i = 0; i < NUM_CHILD; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) {
            int prio = base_priority;
            if (!mode_equal) {
                if (i == 1) prio = base_priority + 5;
                if (i == 2) prio = base_priority - 5;
            }
            run_child(i + 1, prio);
        }
    }

    for (int i = 0; i < NUM_CHILD; i++) {
        int status;
        pid_t finished = wait(&status);
        if (WIFEXITED(status)) {
            printf("Родитель: процесс с PID %d завершился, код возврата %d\n", finished, WEXITSTATUS(status));
        }
    }

    printf("Родитель: все дочерние процессы завершены.\n");
    return 0;
}
