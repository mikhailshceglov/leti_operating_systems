// Цель: Демонстрация работы с именованным каналом (FIFO).
// Командная строка:
//   $ mkfifo /tmp/myfifo                  # создаём FIFO
//   $ echo "Привет из CLI" > /tmp/myfifo # записываем сообщение
//   $ cat /tmp/myfifo                     # читаем сообщение
//   $ ls -l /tmp/myfifo                   # тип файла и права
//   $ stat /tmp/myfifo                    # подробная информация о FIFO
//   $ ls -l /proc/$$/fd | grep myfifo     # отображение дескрипторов

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "/tmp/myfifo"
#define MSG       "Hello from program FIFO!"

void print_stat(const char *path) {
    struct stat st;
    if (stat(path, &st) < 0) {
        perror("stat"); return;
    }
    printf("\nInfo for %s:\n", path);
    printf("  Device: %ld\n", (long)st.st_dev);
    printf("  Inode: %ld\n", (long)st.st_ino);
    printf("  Mode: %o", st.st_mode & 07777);
    printf(" (%s)\n", S_ISFIFO(st.st_mode) ? "FIFO" : "Other");
    printf("  Link count: %ld\n", (long)st.st_nlink);
    printf("  Size: %ld bytes (always 0 for FIFO)\n", (long)st.st_size);
}

int main() {
    // 1. Создать FIFO (если ещё нет)
    if (mkfifo(FIFO_PATH, 0666) < 0) {
        // если уже существует — продолжаем
    }

    // 2. Показать информацию о FIFO до операций
    system("ls -l " FIFO_PATH);
    system("stat " FIFO_PATH);

    print_stat(FIFO_PATH);

    // 3. Fork: читатель и писатель
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        // Child: Writer
        int fdw = open(FIFO_PATH, O_WRONLY);
        if (fdw < 0) { perror("open write"); exit(1); }
        write(fdw, MSG, strlen(MSG));
        close(fdw);
        return 0;
    } else {
        // Parent: Reader
        int fdr = open(FIFO_PATH, O_RDONLY);
        if (fdr < 0) { perror("open read"); exit(1); }

        // Показать через /proc/self/fd
        printf("\n/proc/self/fd entry:\n");
        system("ls -l /proc/$$/fd | grep myfifo");

        char buf[256] = {0};
        read(fdr, buf, sizeof(buf)-1);
        printf("\nRead from FIFO: '%s'\n", buf);
        close(fdr);

        // Удалим FIFO после использования
        unlink(FIFO_PATH);
    }
    return 0;
}
