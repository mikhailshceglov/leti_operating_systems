/*
 * Программа используется в связке с exec() и демонстрирует, что файловые дескрипторы,
 * открытые в родительском процессе, могут быть унаследованы дочерним после exec.
 * Пишет в файловый дескриптор 3, считая его уже открытым до exec().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    dprintf(3, "Child_exec (PID=%d): получил inherited fd (3) после exec(), fd открыт.\n", getpid());
    sleep(2);
    dprintf(3, "Child_exec (PID=%d): завершает работу.\n", getpid());
    return 0;
}
