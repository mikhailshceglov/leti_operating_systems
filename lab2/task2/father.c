/*
 * Программа родителя, демонстрирующая создание дочернего процесса с последующей заменой его образа
 * с помощью execl(). Родитель в это время выполняет фиксирование таблицы процессов в файл и ожидает завершения потомка.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        execl("./son", "son", (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        printf("Родительский процесс: PID = %d, ожидание завершения дочернего процесса (PID = %d)...\n", getpid(), pid);
        sleep(1);
        system("ps -l > process_log.txt");
        system("ps aux > process_aux_log.txt");

        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился со статусом %d\n", WEXITSTATUS(status));
        } else {
            printf("Дочерний процесс завершился ненормально.\n");
        }
        printf("Родительский процесс завершается.\n");
    }

    return 0;
}
