// Цель: Обеспечить разделяемую память и синхронизацию между родительским и дочерним процессами.
//       Родитель записывает сообщения в общую область, дочерний читает их по готовности,
//       без потери данных и без busy-wait. Используются POSIX shared memory и POSIX-семафоры.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string.h>
#include <sys/wait.h>

#define SHM_NAME   "/my_shm"
#define MSG_COUNT  5
#define BUF_SIZE   256

typedef struct {
    sem_t empty;
    sem_t full;
    char buffer[BUF_SIZE];
} SharedData;

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd < 0) {
        perror("shm_open");
        exit(1);
    }
    if (ftruncate(fd, sizeof(SharedData)) < 0) {
        perror("ftruncate");
        exit(1);
    }
    SharedData *shm_ptr = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fd);

    // Инициализация семафоров в разделяемой памяти
    if (sem_init(&shm_ptr->empty, 1, 1) < 0) {
        perror("sem_init empty"); exit(1);
    }
    if (sem_init(&shm_ptr->full,  1, 0) < 0) {
        perror("sem_init full"); exit(1);
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Дочерний процесс: Consumer
        for (int i = 1; i <= MSG_COUNT; ++i) {
            sem_wait(&shm_ptr->full);
            printf("Consumer прочитал: \"%s\"\n", shm_ptr->buffer);
            sem_post(&shm_ptr->empty);
        }
        // Очистка
        munmap(shm_ptr, sizeof(SharedData));
        exit(0);
    } else {
        // Родительский процесс: Producer
        for (int i = 1; i <= MSG_COUNT; ++i) {
            char message[BUF_SIZE];
            snprintf(message, BUF_SIZE, "Сообщение %d", i);
            sem_wait(&shm_ptr->empty);
            strncpy(shm_ptr->buffer, message, BUF_SIZE);
            printf("Producer записал: \"%s\"\n", message);
            sem_post(&shm_ptr->full);
            sleep(1);
        }
        wait(NULL);
        // Дестрой семафоров и разделяемой памяти
        sem_destroy(&shm_ptr->empty);
        sem_destroy(&shm_ptr->full);
        munmap(shm_ptr, sizeof(SharedData));
        shm_unlink(SHM_NAME);
        return 0;
    }
}
