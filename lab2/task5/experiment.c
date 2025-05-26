/*
 * Программа создаёт три дочерних процесса, каждый из которых выполняет 10 итераций с паузой.
 * Родительский процесс ожидает завершения каждого из них и выводит статус завершения.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define NUM_CHILD 3
#define ITERATIONS 10

int main(void) {
    pid_t pids[NUM_CHILD];

    for (int i = 0; i < NUM_CHILD; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) {
            for (int j = 0; j < ITERATIONS; j++) {
                printf("Дочерний процесс %d (PID=%d): итерация %d\n", i + 1, getpid(), j);
                usleep(100000);
            }
            exit(i + 1);
        }
    }

    for (int i = 0; i < NUM_CHILD; i++) {
        int status;
        pid_t finished = wait(&status);
        if (WIFEXITED(status)) {
            printf("Родитель: процесс с PID %d завершился, код возврата: %d\n", finished, WEXITSTATUS(status));
        }
    }
    return 0;
}
