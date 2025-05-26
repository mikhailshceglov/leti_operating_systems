
// Цель: Демонстрация управления потоком с использованием SIGUSR1 без пользовательского обработчика.
// Создает рабочий поток, который печатает сообщение каждые 0,5 секунды.
// Использует системный таймер для отправки SIGUSR1 из основного потока
// в рабочий поток через 2 секунды, отмечает ожидаемое время удаления,
// и замечает, что по умолчанию SIGUSR1 завершит весь процесс.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <sys/time.h>

static pthread_t t2;
static struct timespec start_time;

// Функция для получения относительного времени от старта
void print_rel_time(const char* prefix) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    long sec  = now.tv_sec  - start_time.tv_sec;
    long nsec = now.tv_nsec - start_time.tv_nsec;
    if (nsec < 0) { sec--; nsec += 1000000000L; }
    printf("[%2lds.%09lds] %s\n", sec, nsec, prefix);
}

// Второй поток без своего обработчика
void* second_thread_func(void* arg) {
    int count = 0;
    while (1) {
        char buf[64];
        snprintf(buf, sizeof(buf), "Second thread working cycle %d", ++count);
        print_rel_time(buf);
        usleep(500000);
    }
    return NULL;
}

// SIGALRM‑обработчик в главном потоке — шлёт SIGUSR1 и отмечает предполагаемое время удаления
void alarm_handler(int signo) {
    print_rel_time("Main: timer expired, expected deletion of second thread");
    pthread_kill(t2, SIGUSR1);
}

int main() {
    // Засекаем момент старта
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    // Создаем второй поток
    if (pthread_create(&t2, NULL, second_thread_func, NULL) != 0) {
        perror("pthread_create");
        exit(1);
    }

    // Настраиваем системный таймер ITIMER_REAL на 2 секунды
    struct itimerval timer = {
        .it_value = { .tv_sec = 2, .tv_usec = 0 },
        .it_interval = {0, 0}
    };
    signal(SIGALRM, alarm_handler);
    if (setitimer(ITIMER_REAL, &timer, NULL) < 0) {
        perror("setitimer");
        exit(1);
    }

    // Ждем сигнала и прекращения программы по умолчанию
    pause();
    return 0;
}
