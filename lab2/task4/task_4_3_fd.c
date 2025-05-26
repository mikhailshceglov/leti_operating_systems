/* task_4_3_fd.c - Демонстрация наследования общего адресного пространства и файловых дескрипторов между потоками.
   Все потоки используют общий файловый дескриптор (fd 3, дублированный от STDOUT) для вывода в консоль,
   а также работают с одной глобальной переменной. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <string.h>

#define NUM_THREADS 3
int global_var = 12345;

void *thread_func(void *arg) {
    int thread_num = *(int *)arg;
    char buffer[256];
    int n = sprintf(buffer, "Поток %d (TID=%lu): global_var address=%p, значение=%d\n",
                    thread_num, (unsigned long)pthread_self(), (void *)&global_var, global_var);
    ssize_t written = write(3, buffer, n);
    if (written < 0) {
        perror("write");
    }
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "echo \"FD list for PID %d:\"; ls -l /proc/%d/fd", getpid(), getpid());
    system(cmd);
    return NULL;
}

int main(void) {
    dup2(STDOUT_FILENO, 3);
    pthread_t threads[NUM_THREADS];
    int thread_nums[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_nums[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_func, &thread_nums[i]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    char final[256];
    sprintf(final, "Главный поток: окончательное значение global_var = %d, адрес=%p\n", global_var, (void *)&global_var);
    write(3, final, strlen(final));
    char main_cmd[256];
    snprintf(main_cmd, sizeof(main_cmd), "echo \"FD list for PID %d (main):\"; ls -l /proc/%d/fd", getpid(), getpid());
    system(main_cmd);
    close(3);
    return 0;
}
