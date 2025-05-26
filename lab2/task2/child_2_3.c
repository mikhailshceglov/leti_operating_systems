/* 
 * Программа является дочерним процессом и используется для экспериментов с функциями exec.
 * При запуске она:
 * - выводит свой PID и PPID;
 * - отображает переданные аргументы (argv);
 * - выводит первые 5 переменных окружения (envp).
 * 
 * Завершает выполнение с задержкой, чтобы можно было отследить её состояние снаружи.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
    printf("Child Process started: PID = %d, PPID = %d\n", getpid(), getppid());

    printf("Received Arguments (argc = %d):\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("  argv[%d] = %s\n", i, argv[i]);
    }

    printf("First few Environment Variables:\n");
    for (int i = 0; envp[i] != NULL && i < 5; i++) {
        printf("  envp[%d] = %s\n", i, envp[i]);
    }

    sleep(2);
    printf("Child Process (PID = %d) exiting...\n", getpid());
    return 0;
}
