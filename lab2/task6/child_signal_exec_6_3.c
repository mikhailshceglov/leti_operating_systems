/*
 * Программа запускается после exec и выводит текущие диспозиции сигналов SIGUSR1 и SIGUSR2.
 * Показывает, унаследованы ли настройки сигналов (default, ignore, custom handler).
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void print_signal_action(int signum, const char *signame) {
    struct sigaction act;
    if (sigaction(signum, NULL, &act) < 0) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }
    if (act.sa_handler == SIG_DFL) {
        printf("%s: default\n", signame);
    } else if (act.sa_handler == SIG_IGN) {
        printf("%s: ignored\n", signame);
    } else {
        printf("%s: custom handler at %p\n", signame, (void *)act.sa_handler);
    }
}

int main(void) {
    printf("Child_exec (PID=%d): In exec child, checking inherited signal dispositions...\n", getpid());
    print_signal_action(SIGUSR1, "SIGUSR1");
    print_signal_action(SIGUSR2, "SIGUSR2");
    printf("Child_exec (PID=%d): Exiting.\n", getpid());
    return 0;
}
