/*
 * Программа дочернего процесса, предназначенная для запуска через exec-функции.
 * Выводит PID и PPID, делает паузу для фиксации в таблице процессов и завершает выполнение.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("Дочерний процесс запущен: PID = %d, PPID = %d\n", getpid(), getppid());
    sleep(5);
    printf("Дочерний процесс завершается.\n");
    exit(EXIT_SUCCESS);
}
