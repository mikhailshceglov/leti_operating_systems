/*
 * Программа демонстрирует возможность завершения отдельного потока с помощью сигнала.
 * Один из потоков (№2) устанавливает обработчик сигнала SIGUSR1. При получении этого сигнала поток завершает работу,
 * остальные продолжают выполнение. Основной процесс запускает потоки и даёт пользователю время для эксперимента.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/syscall.h>

void sig_handler(int sig) {
    printf("Поток с TID=%ld получил сигнал %d и завершает работу.\n", syscall(SYS_gettid), sig);
    pthread_exit(NULL);
}

void* thread_func(void* arg) {
    int num = *(int*)arg;
    printf("Поток #%d запущен: PID=%d, TID=%ld\n", num, getpid(), syscall(SYS_gettid));
    if (num == 2) {
        if (signal(SIGUSR1, sig_handler) == SIG_ERR) {
            perror("signal");
            pthread_exit(NULL);
        }
        printf("Поток #%d: установлен обработчик для SIGUSR1.\n", num);
    }
    while(1) {
        sleep(1);
    }
    return NULL;
}

int main(void) {
    pthread_t threads[2];
    int nums[2] = {1, 2};

    if(pthread_create(&threads[0], NULL, thread_func, &nums[0]) != 0) {
        perror("pthread_create для потока 1");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&threads[1], NULL, thread_func, &nums[1]) != 0) {
        perror("pthread_create для потока 2");
        exit(EXIT_FAILURE);
    }

    printf("Главный процесс: PID=%d\n", getpid());
    printf("Запущены два потока. Проверьте их TID с помощью команды: ps -eLf | grep <имя вашей программы>\n");
    printf("Чтобы попытаться удалить (завершить) поток с кастомным обработчиком, откройте новый терминал и выполните:\n");
    printf("    kill -SIGUSR1 <TID_потока_2>\n");
    printf("Обратите внимание: отправка сигнала, действие которого по умолчанию завершает процесс (например, SIGTERM), приведёт к завершению всего процесса, а не только выбранного потока.\n");

    sleep(30);

    printf("Главный процесс завершает работу.\n");
    exit(0);
}
