/*
 * Программа демонстрирует, как сигнал SIGUSR1 с пользовательским обработчиком и SIGUSR2 с игнорированием
 * наследуются дочерними процессами при fork() и что происходит с ними после exec().
 * Один потомок остаётся после fork(), второй выполняет exec нового процесса.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void custom_handler(int sig) {
    printf("Custom handler triggered in process %d for signal %d\n", getpid(), sig);
}

int main(int argc, char *argv[]) {
    if (signal(SIGUSR1, custom_handler) == SIG_ERR) {
        perror("signal SIGUSR1");
        exit(EXIT_FAILURE);
    }
    if (signal(SIGUSR2, SIG_IGN) == SIG_ERR) {
        perror("signal SIGUSR2");
        exit(EXIT_FAILURE);
    }

    printf("Parent (PID=%d): Installed custom handler for SIGUSR1 and ignoring SIGUSR2.\n", getpid());

    pid_t child1 = fork();
    if (child1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child1 == 0) {
        printf("Child1 (PID=%d): Inherited signal dispositions via fork.\n", getpid());
        sleep(1);
        printf("Child1 (PID=%d): Raising SIGUSR1...\n", getpid());
        raise(SIGUSR1);
        printf("Child1 (PID=%d): Raising SIGUSR2...\n", getpid());
        raise(SIGUSR2);
        sleep(1);
        printf("Child1 (PID=%d): Exiting normally (fork mode).\n", getpid());
        exit(1);
    }

    pid_t child2 = fork();
    if (child2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child2 == 0) {
        printf("Child2 (PID=%d): About to exec child_signal_exec_6_3; signal dispositions should reset for caught signals.\n", getpid());
        execl("./child_signal_exec_6_3", "child_signal_exec_6_3", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    wait(NULL);
    printf("Parent (PID=%d): Both children finished.\n", getpid());
    return 0;
}
