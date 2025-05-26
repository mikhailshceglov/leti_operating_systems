/*
 * Программа демонстрирует наследование файлового дескриптора при fork() и exec().
 * Родитель открывает файл и передаёт его дескриптор дочернему процессу.
 * В зависимости от режима (fork/exec), дочерний пишет напрямую или через exec-программу.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int fd = open("inherit_file.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    dprintf(fd, "Parent (PID=%d): Файл открыт.\n", getpid());

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        dprintf(fd, "Child (PID=%d, PPID=%d): наследует fd до exec().\n", getpid(), getppid());

        if (argc > 1 && strcmp(argv[1], "exec") == 0) {
            if (fd != 3) {
                if (dup2(fd, 3) == -1) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(fd);
                fd = 3;
            }
            dprintf(fd, "Child (PID=%d): перед exec(), fd еще открыт.\n", getpid());
            execl("./child_exec", "child_exec", NULL);
            perror("execl");
            exit(EXIT_FAILURE);
        } else {
            dprintf(fd, "Child (PID=%d): работает после fork(), fd открыт.\n", getpid());
            sleep(2);
            dprintf(fd, "Child (PID=%d): завершает работу (fork mode).\n", getpid());
            close(fd);
            exit(EXIT_SUCCESS);
        }
    } else {
        wait(NULL);
        dprintf(fd, "Parent (PID=%d): Потомок завершился.\n", getpid());
        close(fd);
        printf("Эксперимент завершён. Проверьте файл inherit_file.log\n");
    }

    return 0;
}
