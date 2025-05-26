/*
 * Эта программа демонстрирует три сценария работы процессов:
 * a) Родительский процесс ожидает завершения дочернего;
 * b) Родитель завершает выполнение до завершения дочернего, в результате чего дочерний переназначается init-процессу;
 * c) Родитель не ожидает завершения дочернего сразу, из-за чего последний становится "зомби", 
 *    пока родитель не вызовет wait().
 *
 * Используется fork(), waitpid(), sleep() для демонстрации поведения процессов.
 * Программа принимает аргумент командной строки: a, b, c или all для запуска соответствующего сценария.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void scenario_a() {
    printf("Эксперимент a: процесс-отец запускает сына и ОЖИДАЕТ его завершения.\n");
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("[a] Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(0);
    } else {
        printf("[a] Родительский процесс: PID = %d, ожидает ребенка (PID = %d)...\n", getpid(), pid);
        int status;
        waitpid(pid, &status, 0);
        printf("[a] Родительский процесс: Дочерний завершился со статусом %d\n", WEXITSTATUS(status));
    }
}

void scenario_b() {
    printf("Эксперимент b: процесс-отец запускает сына и НЕ ожидает его завершения, затем завершается сам.\n");
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("[b] Дочерний процесс: PID = %d, PPID = %d (до ухода отца)\n", getpid(), getppid());
        sleep(3);
        printf("[b] Дочерний процесс: PID = %d, PPID = %d (после ухода родителя)\n", getpid(), getppid());
        exit(0);
    } else {
        printf("[b] Родительский процесс: PID = %d. Завершается немедленно без ожидания ребенка (PID = %d).\n", getpid(), pid);
        exit(0);
    }
}

void scenario_c() {
    printf("Эксперимент c: процесс-отец запускает сына и НЕ ожидает его завершения; потом ребенок завершается, остаётся zombie, затем родитель вызывает wait() для его очистки.\n");
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        printf("[c] Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        exit(0);
    } else {
        printf("[c] Родительский процесс: PID = %d. Не вызывает wait() сразу, чтобы продемонстрировать состояние zombie (PID = %d).\n", getpid(), pid);
        sleep(3);
        int status;
        waitpid(pid, &status, 0);
        printf("[c] Родительский процесс: Дочерний процесс (PID = %d) завершен и зомби очищен.\n", pid);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [a|b|c|all]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "a") == 0 || strcmp(argv[1], "all") == 0)
        scenario_a();
    if (strcmp(argv[1], "b") == 0 || strcmp(argv[1], "all") == 0)
        scenario_b();
    if (strcmp(argv[1], "c") == 0 || strcmp(argv[1], "all") == 0)
        scenario_c();

    return 0;
}
