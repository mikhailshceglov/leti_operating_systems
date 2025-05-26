/*
 * Программа демонстрирует параллельное выполнение родительского и дочернего процессов.
 * Каждый процесс выполняет циклические вычисления с задержкой, что позволяет наблюдать 
 * конкуренцию за процессорное время. Родитель ожидает завершения дочернего.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LOOP_COUNT 50
#define SLEEP_USEC 50000

int main(void) {
    pid_t pid;

    printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    pid = fork();
    if (pid < 0) {
        perror("Ошибка fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        for (int i = 0; i < LOOP_COUNT; i++) {
            int temp = i * i;
            printf("Дочерний [PID %d]: Итерация %d, значение %d\n", getpid(), i, temp);
            usleep(SLEEP_USEC);
        }
        exit(EXIT_SUCCESS);
    } else {
        printf("Родительский процесс продолжает работу, ожидая завершения дочернего (PID = %d)...\n", pid);
        for (int i = 0; i < LOOP_COUNT; i++) {
            int temp = i * 2;
            printf("Родительский [PID %d]: Итерация %d, значение %d\n", getpid(), i, temp);
            usleep(SLEEP_USEC);
        }
        int status;
        wait(&status);
        printf("Родительский процесс: Дочерний завершился со статусом %d\n", WEXITSTATUS(status));
    }

    return 0;
}
