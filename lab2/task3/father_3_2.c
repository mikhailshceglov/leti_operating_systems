/*
 * Программа демонстрирует отслеживание завершения нескольких дочерних процессов
 * с использованием wait() и waitpid(). Родитель обрабатывает их завершения
 * в разной последовательности и отслеживает состояние зомби-процессов.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t son1, son2, son3;
    int status;

    if ((son1 = fork()) == 0) {
        printf("Son1 (PID=%d) started.\n", getpid());
        sleep(2);
        exit(1);
    }

    if ((son2 = fork()) == 0) {
        printf("Son2 (PID=%d) started.\n", getpid());
        sleep(4);
        exit(2);
    }

    if ((son3 = fork()) == 0) {
        printf("Son3 (PID=%d) started.\n", getpid());
        sleep(6);
        exit(3);
    }

    pid_t finished_pid = wait(&status);
    if (WIFEXITED(status)) {
        printf("Родитель: потомок %d завершился первым, код возврата: %d\n", finished_pid, WEXITSTATUS(status));
    }

    waitpid(son3, &status, 0);
    if (WIFEXITED(status)) {
        printf("Родитель: потомок son3 (PID=%d) завершился, код возврата: %d\n", son3, WEXITSTATUS(status));
    }

    system("ps -l | grep Z");

    waitpid(son2, &status, 0);
    if (WIFEXITED(status)) {
        printf("Родитель: потомок son2 (PID=%d) завершился, код возврата: %d\n", son2, WEXITSTATUS(status));
    }

    printf("Все потомки завершены, программа завершается.\n");
    return 0;
}
