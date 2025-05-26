/*
 * Программа запускает три дочерних процесса, каждый из которых по-разному обрабатывает сигнал SIGUSR1.
 * Затем родитель отправляет этот сигнал каждому процессу, фиксирует состояние системы до и после
 * отправки сигналов, и завершает оставшиеся процессы. Используется execl, kill, wait и ps.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

void log_ps(const char *label, const char *filename) {
    printf("\n--- %s ---\n", label);
    char command[256];
    snprintf(command, sizeof(command), "echo \"\n--- %s ---\n\" >> %s; ps -s >> %s", label, filename, filename);
    system(command);
}

int main(void) {
    pid_t son1, son2, son3;

    if ((son1 = fork()) == 0) {
        execl("./son1", "son1", NULL);
        perror("execl son1");
        exit(EXIT_FAILURE);
    }

    if ((son2 = fork()) == 0) {
        execl("./son2", "son2", NULL);
        perror("execl son2");
        exit(EXIT_FAILURE);
    }

    if ((son3 = fork()) == 0) {
        execl("./son3", "son3", NULL);
        perror("execl son3");
        exit(EXIT_FAILURE);
    }

    sleep(2);

    log_ps("До отправки сигналов", "ps_before.txt");

    kill(son1, SIGUSR1);
    kill(son2, SIGUSR1);
    kill(son3, SIGUSR1);

    sleep(2);

    log_ps("После отправки сигналов", "ps_after.txt");

    kill(son2, SIGTERM);
    kill(son3, SIGTERM);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("Father process: Все дочерние процессы завершены.\n");
    return 0;
}
