/*
 * Программа демонстрирует поведение глобальной переменной при создании дочернего процесса через clone().
 * В режиме "private" используется fork-подобное поведение (раздельное адресное пространство),
 * в режиме "shared" — адресное пространство общее, что позволяет наблюдать изменение переменной глобально.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <string.h>

#define STACK_SIZE (1024 * 1024)

int global_var = 100;

int clone_child(void *arg) {
    char *mode = (char *)arg;
    printf("Clone child: PID=%d, global_var address=%p, initial value=%d (mode=%s)\n",
           getpid(), (void*)&global_var, global_var, mode);
    global_var += 50;
    printf("Clone child: PID=%d, updated global_var = %d (mode=%s)\n",
           getpid(), global_var, mode);
    return 0;
}

int main(int argc, char **argv) {
    void *child_stack = malloc(STACK_SIZE);
    if (child_stack == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    printf("Parent: PID=%d, global_var address=%p, initial value=%d\n",
           getpid(), (void*)&global_var, global_var);

    int flags;
    char *mode;
    if (argc > 1 && strcmp(argv[1], "private") == 0) {
        flags = SIGCHLD;
        mode = "private";
    } else {
        flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD;
        mode = "shared";
    }

    pid_t child_pid = clone(clone_child, child_stack + STACK_SIZE, flags, mode);
    if (child_pid == -1) {
        perror("clone");
        free(child_stack);
        exit(EXIT_FAILURE);
    }

    waitpid(child_pid, NULL, 0);
    printf("Parent after child (mode=%s): global_var = %d\n", mode, global_var);
    free(child_stack);
    return 0;
}
