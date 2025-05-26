// Цель: Демонстрация программного использования анонимного канала pipe().
// Создаёт неименованный канал, выводит его максимальный размер буфера через fcntl(F_GETPIPE_SZ),
// затем через fork() демонстрирует запись в канал в дочернем процессе и чтение в родительском.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main() {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Получаем размер буфера канала
    int bufsize = fcntl(pipefd[1], F_GETPIPE_SZ);
    if (bufsize < 0) {
        perror("fcntl F_GETPIPE_SZ");
    } else {
        printf("Default pipe buffer size: %d bytes\n", bufsize);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс: пишет в канал
        close(pipefd[0]); // закрываем конец для чтения
        const char *msg = "Hello through pipe!";
        write(pipefd[1], msg, strlen(msg));
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    } else {
        // Родительский процесс: читает из канала
        close(pipefd[1]); // закрываем конец для записи
        char buffer[128] = {0};
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer)-1);
        if (n < 0) {
            perror("read");
        } else {
            printf("Read from pipe: '%s'\n", buffer);
        }
        close(pipefd[0]);
        return 0;
    }
}
