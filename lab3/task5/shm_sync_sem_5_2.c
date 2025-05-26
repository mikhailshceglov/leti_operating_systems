// Цель: Синхронизация через POSIX‑семафоры в разделяемой памяти.
//       Измерение времени доставки MSG_COUNT сообщений и расчёт пропускной способности.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SHM_NAME   "/my_shm_sem"
#define MSG_COUNT 100000
#define BUF_SIZE   1

typedef struct {
    sem_t empty;
    sem_t full;
    char buffer[BUF_SIZE];
} SharedData;

long diff_us(struct timeval s, struct timeval e) {
    return (e.tv_sec - s.tv_sec) * 1000000L + (e.tv_usec - s.tv_usec);
}

int main() {
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0600);
    ftruncate(fd, sizeof(SharedData));
    SharedData *shm = mmap(NULL, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sem_init(&shm->empty, 1, 1);
    sem_init(&shm->full,  1, 0);

    pid_t pid = fork();
    struct timeval start, end;

    if (pid > 0) {
        // Producer
        gettimeofday(&start, NULL);
        for (int i = 0; i < MSG_COUNT; ++i) {
            sem_wait(&shm->empty);
            shm->buffer[0] = (char)i;
            sem_post(&shm->full);
        }
        wait(NULL);
        gettimeofday(&end, NULL);
        long us = diff_us(start, end);
        double secs = us / 1e6;
        double throughput = MSG_COUNT / secs;
        printf("Semaphore sync: %ld us, %.2f msgs/sec\n", us, throughput);

        sem_destroy(&shm->empty);
        sem_destroy(&shm->full);
        munmap(shm, sizeof(*shm));
        shm_unlink(SHM_NAME);
    } else {
        // Consumer
        for (int i = 0; i < MSG_COUNT; ++i) {
            sem_wait(&shm->full);
            sem_post(&shm->empty);
        }
        munmap(shm, sizeof(*shm));
        exit(0);
    }
    return 0;
}
