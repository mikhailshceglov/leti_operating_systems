/*
 * Программа сравнивает создание потоков двумя способами: через pthread_create и через системный вызов clone.
 * Каждый поток выводит свой PID и TID. Также производится вывод таблицы потоков через ps -eLf изнутри программы.
 * В завершение происходит ожидание pthread-потоков и пауза для анализа clone-потоков.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>

#define STACK_SIZE (1024 * 1024)

void* pthread_thread_func(void* arg) {
    int thread_num = *(int*)arg;
    printf("pthread поток №%d: PID=%d, TID=%ld\n", thread_num, getpid(), syscall(SYS_gettid));
    sleep(2);
    return NULL;
}

int clone_thread_func(void* arg) {
    int thread_num = *(int*)arg;
    printf("clone поток №%d: PID=%d, TID=%ld\n", thread_num, getpid(), syscall(SYS_gettid));
    sleep(2);
    return 0;
}

int main() {
    pthread_t pthread1, pthread2;
    int thread_nums[4] = {1, 2, 3, 4};

    pthread_create(&pthread1, NULL, pthread_thread_func, &thread_nums[0]);
    pthread_create(&pthread2, NULL, pthread_thread_func, &thread_nums[1]);

    char *stack1 = malloc(STACK_SIZE);
    char *stack2 = malloc(STACK_SIZE);
    if (!stack1 || !stack2) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    pid_t clone1 = clone(clone_thread_func, stack1 + STACK_SIZE,
                         CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD,
                         &thread_nums[2]);
    if (clone1 == -1) {
        perror("clone1");
        exit(EXIT_FAILURE);
    }

    pid_t clone2 = clone(clone_thread_func, stack2 + STACK_SIZE,
                         CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD,
                         &thread_nums[3]);
    if (clone2 == -1) {
        perror("clone2");
        exit(EXIT_FAILURE);
    }

    printf("Главный поток (PID=%d, TID=%ld)\n", getpid(), syscall(SYS_gettid));
    sleep(1);

    printf(">>> Печатаю потоки через ps изнутри программы <<<\n");
    system("ps -eLf | grep task_4_1");

    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);

    printf("Пауза 30 секунд для анализа через ps…\n");
    sleep(30);

    free(stack1);
    free(stack2);

    printf("Все потоки завершены.\n");
    return 0;
}
