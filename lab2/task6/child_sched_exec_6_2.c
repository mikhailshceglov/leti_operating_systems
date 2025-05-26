/*
 * Программа выводит параметры планирования текущего процесса после запуска через exec.
 * Используется sched_getscheduler() и sched_getparam() для определения политики и приоритета.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

void print_sched_info(const char *label) {
    int policy = sched_getscheduler(0);
    if (policy < 0) {
        perror("sched_getscheduler");
        return;
    }
    struct sched_param param;
    if (sched_getparam(0, &param) < 0) {
        perror("sched_getparam");
        return;
    }

    const char *policy_str;
    switch(policy) {
        case SCHED_OTHER: policy_str = "SCHED_OTHER"; break;
        case SCHED_FIFO:  policy_str = "SCHED_FIFO"; break;
        case SCHED_RR:    policy_str = "SCHED_RR"; break;
        default:          policy_str = "UNKNOWN"; break;
    }
    printf("%s: PID=%d, Policy=%s, Priority=%d\n", label, getpid(), policy_str, param.sched_priority);
}

int main(void) {
    printf("Child (exec) process scheduling parameters:\n");
    print_sched_info("Child (exec)");
    return 0;
}
