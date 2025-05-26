#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void handle_sigusr1(int sig) {
    printf("Son3 (PID=%d): Перехватил сигнал %d (SIGUSR1).\n", getpid(), sig);
    system("echo '\n--- Son3 после сигнала ---\n' >> after_signal.txt; ps -s >> after_signal.txt");
}

int main(void) {
    printf("Son3 (PID=%d): Запущен, установлен обработчик для SIGUSR1.\n", getpid());
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    while(1) {
        sleep(1);
    }
    return 0;
}
