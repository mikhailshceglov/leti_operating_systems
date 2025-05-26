/*
 * Программа демонстрирует изменение приоритета текущего процесса с помощью вызова nice().
 * Получает значение nice до и после увеличения на 5 и выводит их на экран.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    int p = getpriority(PRIO_PROCESS, 0);
    if (p == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("Начальное значение nice: %d\n", p);

    int ret = nice(5);
    if (ret == -1 && errno != 0) {
        perror("nice");
        exit(EXIT_FAILURE);
    }

    p = getpriority(PRIO_PROCESS, 0);
    if (p == -1 && errno != 0) {
        perror("getpriority");
        exit(EXIT_FAILURE);
    }
    printf("Значение nice после вызова nice(5): %d\n", p);

    return 0;
}
