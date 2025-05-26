/*
 * Программа демонстрирует, как политики и приоритеты планирования наследуются при fork() и при exec().
 * Один дочерний процесс выводит параметры после fork(), другой — после exec() другой программы.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

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
    switch (policy) {
        case SCHED_OTHER: policy_str = "SCHED_OTHER"; break;
        case SCHED_FIFO:  policy_str = "SCHED_FIFO"; break;
        case SCHED_RR:    policy_str = "SCHED_RR"; break;
        default:          policy_str = "UNKNOWN"; break;
    }
    printf("%s: PID=%d, Policy=%s, Priority=%d\n", label, getpid(), policy_str, param.sched_priority);
}

int main(int argc, char *argv[]) {
    printf("Parent process scheduling parameters:\n");
    print_sched_info("Parent");

    pid_t child1 = fork();
    if (child1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child1 == 0) {
        print_sched_info("Child (fork)");
        exit(EXIT_SUCCESS);
    }

    pid_t child2 = fork();
    if (child2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (child2 == 0) {
        execl("./child_sched_exec_6_2", "child_sched_exec_6_2", NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    wait(NULL);
    wait(NULL);
    return 0;
}
