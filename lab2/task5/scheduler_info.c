/*
 * Программа определяет и выводит текущую политику планирования процесса
 * (SCHED_OTHER, SCHED_FIFO или SCHED_RR), а также текущее значение приоритета (nice).
 */

#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <errno.h>

int main(void) {
    int policy = sched_getscheduler(0);
    if (policy == -1) {
        perror("sched_getscheduler");
        exit(EXIT_FAILURE);
    }

    printf("Текущая политика планирования: ");
    switch(policy) {
        case SCHED_OTHER:
            printf("SCHED_OTHER (обычная)\n");
            break;
        case SCHED_FIFO:
            printf("SCHED_FIFO (FIFO)\n");
            break;
        case SCHED_RR:
            printf("SCHED_RR (Round Robin)\n");
            break;
        default:
            printf("Другой (%d)\n", policy);
    }

    int nice_val = getpriority(PRIO_PROCESS, 0);
    if (nice_val == -1 && errno) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("Текущее значение nice: %d\n", nice_val);

    return 0;
}
