#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int main(void) {
    printf("Son2 (PID=%d): Запущен, игнорирует сигнал SIGUSR1.\n", getpid());
    signal(SIGUSR1, SIG_IGN);
    while(1) {
        sleep(1);
    }
    return 0;
}
