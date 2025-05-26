#include <stdio.h>
#include <unistd.h>

int main(void) {
    printf("Son1 (PID=%d): Запущен, реакция по умолчанию.\n", getpid());
    while(1) {
        sleep(1);
    }
    return 0;

}
