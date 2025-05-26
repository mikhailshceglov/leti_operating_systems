#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void handler_int(int signo) {
    printf("Обработчик SIGINT начат.\n");
    // Отправляем SIGUSR1 самому себе во время обработки SIGINT
    printf("Посылаем SIGUSR1 из обработчика SIGINT...\n");
    kill(getpid(), SIGUSR1);
    // Имитируем длительную обработку (спим 1 секунду)
    sleep(1);
    printf("Обработчик SIGINT завершен.\n");
}

void handler_usr1(int signo) {
    printf("Обработчик SIGUSR1 выполнен.\n");
}

int main() {
    struct sigaction act_int, act_usr, old_int;
    // Настраиваем обработчик для SIGINT
    act_int.sa_handler = handler_int;
    sigemptyset(&act_int.sa_mask);
    // Блокируем SIGUSR1 во время обработки SIGINT
    sigaddset(&act_int.sa_mask, SIGUSR1);
    act_int.sa_flags = 0; // без специальных флагов
    // Устанавливаем и сохраняем старое действие
    sigaction(SIGINT, &act_int, &old_int);

    // Настраиваем обработчик для SIGUSR1 (без маскировки)
    act_usr.sa_handler = handler_usr1;
    sigemptyset(&act_usr.sa_mask);
    act_usr.sa_flags = 0;
    sigaction(SIGUSR1, &act_usr, NULL);

    // Проверим, был ли у SIGINT обработчик по умолчанию
    if (old_int.sa_handler == SIG_DFL) {
        printf("Старое действие для SIGINT было SIG_DFL (по умолчанию).\n");
    }

    printf("Посылаем SIGINT самому себе...\n");
    kill(getpid(), SIGINT);

    // Дадим время обработчикам выполниться
    sleep(2);
    printf("Основной процесс завершает работу.\n");
    return 0;
}
