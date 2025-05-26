/*
 * Программа демонстрирует работу различных функций семейства exec (execl, execv, execle, execve, execlp, execvp).
 * Для каждого случая создаётся дочерний процесс, который заменяет своё содержимое на выполнение программы child_2_3.
 * Родитель ожидает завершения каждого дочернего процесса и сообщает о завершении эксперимента.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void run_experiment(int exp_number) {
    pid_t pid = fork();
    if (pid == 0) {
        switch (exp_number) {
            case 1:
                printf("Эксперимент 1: execl() - передача аргументов списком\n");
                if (execl("./child_2_3", "child_2_3", "Arg1", "Arg2", (char *)NULL) == -1) {
                    perror("execl failed");
                }
                exit(EXIT_FAILURE);
            case 2:
                printf("Эксперимент 2: execv() - передача аргументов через массив\n");
                {
                    char *args[] = {"child_2_3", "Arg1", "Arg2", NULL};
                    if (execv("./child_2_3", args) == -1) {
                        perror("execv failed");
                    }
                }
                exit(EXIT_FAILURE);
            case 3:
                printf("Эксперимент 3: execle() - передача аргументов и окружения\n");
                {
                    char *envp[] = {"MYVAR=env_execle", "PATH=.:/usr/bin", NULL};
                    if (execle("./child_2_3", "child_2_3", "Arg1", "Arg2", (char *)NULL, envp) == -1) {
                        perror("execle failed");
                    }
                }
                exit(EXIT_FAILURE);
            case 4:
                printf("Эксперимент 4: execve() - передача аргументов и окружения через массивы\n");
                {
                    char *args[] = {"child_2_3", "Arg1", "Arg2", NULL};
                    char *envp[] = {"MYVAR=env_execve", "PATH=.:/usr/bin", NULL};
                    if (execve("./child_2_3", args, envp) == -1) {
                        perror("execve failed");
                    }
                }
                exit(EXIT_FAILURE);
            case 5:
                printf("Эксперимент 5: execlp() - поиск по PATH\n");
                {
                    setenv("PATH", ".:/usr/bin", 1);
                    if (execlp("child_2_3", "child_2_3", "Arg1", "Arg2", (char *)NULL) == -1) {
                        perror("execlp failed");
                    }
                }
                exit(EXIT_FAILURE);
            case 6:
                printf("Эксперимент 6: execvp() - передача аргументов с поиском по PATH\n");
                {
                    setenv("PATH", ".:/usr/bin", 1);
                    char *args[] = {"child_2_3", "Arg1", "Arg2", NULL};
                    if (execvp("child_2_3", args) == -1) {
                        perror("execvp failed");
                    }
                }
                exit(EXIT_FAILURE);
            default:
                printf("Неизвестный эксперимент\n");
                exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        int status;
        wait(&status);
        printf("Эксперимент %d завершился.\n\n", exp_number);
    } else {
        perror("fork failed");
    }
}

int main(void) {
    printf("Родительский процесс (PID = %d): Начало экспериментов с exec-функциями\n", getpid());
    for (int i = 1; i <= 6; i++) {
        run_experiment(i);
    }
    printf("Родительский процесс (PID = %d): Все эксперименты завершены.\n", getpid());
    return 0;
}
