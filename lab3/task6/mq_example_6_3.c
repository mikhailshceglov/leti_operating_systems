// File: mq_example_6_3.c
// Цель: Показать работу POSIX-очереди сообщений в неблокирующем режиме как «почтового ящика».
//       Потомок шлёт сообщения с задержкой, родитель в цикле пытается получать их без блокировки,
//       выполняя «другую работу» при пустой очереди.

#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>

#define QUEUE_NAME   "/my_queue"
#define MSG_COUNT    5
#define MSG_SIZE     64

int main() {
    struct mq_attr attr = {0};
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_SIZE;
    mqd_t mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) { perror("mq_open"); exit(1); }

    // Вывести атрибуты очереди
    struct mq_attr cur;
    mq_getattr(mq, &cur);
    printf("Queue attributes: maxmsg=%ld, msgsize=%ld, curmsgs=%ld\n",
           cur.mq_maxmsg, cur.mq_msgsize, cur.mq_curmsgs);

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); exit(1); }

    if (pid == 0) {
        // Потомок: отправка сообщений
        for (int i = 1; i <= MSG_COUNT; ++i) {
            char msg[MSG_SIZE];
            snprintf(msg, MSG_SIZE, "Message %d", i);
            mq_send(mq, msg, strlen(msg)+1, 0);
            printf("Child: sent '%s'\n", msg);
            sleep(1);
        }
        mq_close(mq);
        exit(0);
    } else {
        // Родитель: получение без блокировки
        // Переводим дескриптор в неблокирующий режим
        mq_close(mq);
        mq = mq_open(QUEUE_NAME, O_RDONLY | O_NONBLOCK);
        if (mq == (mqd_t)-1) { perror("mq_open nonblock"); exit(1); }

        int received = 0;
        while (received < MSG_COUNT) {
            char buf[MSG_SIZE];
            ssize_t bytes = mq_receive(mq, buf, MSG_SIZE, NULL);
            if (bytes >= 0) {
                printf("Parent: received '%s'\n", buf);
                received++;
            } else if (errno == EAGAIN) {
                printf("Parent: no messages, doing other work...\n");
                usleep(500000);
            } else {
                perror("mq_receive");
                break;
            }
        }
        mq_close(mq);
        mq_unlink(QUEUE_NAME);
    }
    return 0;
}

