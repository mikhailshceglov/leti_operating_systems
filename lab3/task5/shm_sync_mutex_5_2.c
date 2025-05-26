// Цель: Синхронизация через pthread_mutex + pthread_cond в разделяемой памяти.
//       Измерение времени доставки MSG_COUNT сообщений и расчёт пропускной способности.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>

#define SHM_NAME   "/my_shm_mutex"
#define MSG_COUNT 100000
#define BUF_SIZE   1

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    int ready;
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

    pthread_mutexattr_t mattr;
    pthread_condattr_t  cattr;
    pthread_mutexattr_init(&mattr);
    pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
    pthread_condattr_init(&cattr);
    pthread_condattr_setpshared(&cattr, PTHREAD_PROCESS_SHARED);

    pthread_mutex_init(&shm->mutex, &mattr);
    pthread_cond_init(&shm->cond, &cattr);
    shm->ready = 0;

    pid_t pid = fork();
    struct timeval start, end;

    if (pid > 0) {
        // Producer
        gettimeofday(&start, NULL);
        for (int i = 0; i < MSG_COUNT; ++i) {
            pthread_mutex_lock(&shm->mutex);
            shm->buffer[0] = (char)i;
            shm->ready = 1;
            pthread_cond_signal(&shm->cond);
            while (shm->ready)
                pthread_cond_wait(&shm->cond, &shm->mutex);
            pthread_mutex_unlock(&shm->mutex);
        }
        wait(NULL);
        gettimeofday(&end, NULL);
        long us = diff_us(start, end);
        double secs = us / 1e6;
        double throughput = MSG_COUNT / secs;
        printf("Mutex+cond sync: %ld us, %.2f msgs/sec\n", us, throughput);

        pthread_mutex_destroy(&shm->mutex);
        pthread_cond_destroy(&shm->cond);
        munmap(shm, sizeof(*shm));
        shm_unlink(SHM_NAME);
    } else {
        // Consumer
        for (int i = 0; i < MSG_COUNT; ++i) {
            pthread_mutex_lock(&shm->mutex);
            while (!shm->ready)
                pthread_cond_wait(&shm->cond, &shm->mutex);
            shm->ready = 0;
            pthread_cond_signal(&shm->cond);
            pthread_mutex_unlock(&shm->mutex);
        }
        munmap(shm, sizeof(*shm));
        exit(0);
    }
    return 0;
}
