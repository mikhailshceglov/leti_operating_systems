/*
 * Программа выводит допустимые диапазоны приоритетов для политик планирования SCHED_FIFO и SCHED_RR
 * с помощью функций sched_get_priority_min() и sched_get_priority_max().
 */

#include <stdio.h>
#include <sched.h>
#include <stdlib.h>

int main(void) {
    int min_fifo = sched_get_priority_min(SCHED_FIFO);
    int max_fifo = sched_get_priority_max(SCHED_FIFO);
    int min_rr = sched_get_priority_min(SCHED_RR);
    int max_rr = sched_get_priority_max(SCHED_RR);

    if (min_fifo == -1 || max_fifo == -1 || min_rr == -1 || max_rr == -1) {
        perror("sched_get_priority_*");
        exit(EXIT_FAILURE);
    }

    printf("Приоритеты для SCHED_FIFO: минимальный = %d, максимальный = %d\n", min_fifo, max_fifo);
    printf("Приоритеты для SCHED_RR:   минимальный = %d, максимальный = %d\n", min_rr, max_rr);

    return 0;
}
