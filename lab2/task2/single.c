/*
 * Программа демонстрирует порождение дочернего процесса с помощью fork(),
 * выполнение независимых вычислений в каждом процессе и ожидание завершения потомка.
 * Выводит PID/PPID, выполняет простые арифметические операции, а также показывает,
 * как может быть использован wait() для синхронизации.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
        int a = 10, b = 5;
        int sum = a + b;
        printf("Дочерний процесс: %d + %d = %d\n", a, b, sum);
        exit(EXIT_SUCCESS);
    } else {
        printf("Родительский процесс продолжает работу, ожидая завершения дочернего (PID = %d)...\n", pid);
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился со статусом %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершился не нормально.\n");
        }
        int x = 7, y = 3;
        int mult = x * y;
        printf("Родительский процесс: %d * %d = %d\n", x, y, mult);
    }

    printf("Процесс с PID %d: Завершение программы.\n", getpid());
    return 0;
}
