// Цель: Демонстрация локального TCP/IP клиента, подключается к 127.0.0.1:5000, отправляет сообщение и читает ответ.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[BUF_SIZE];

    // 1. Создать TCP-сокет
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); exit(1); }

    // 2. Настроить адрес сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton"); exit(1);
    }

    // 3. Connect
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect"); exit(1);
    }
    printf("Connected to server.\n");

    // 4. Отправить сообщение
    const char *msg = "This message from TCP client!";
    write(sock, msg, strlen(msg));

    // 5. Прочитать ответ
    int n = read(sock, buffer, BUF_SIZE - 1);
    if (n < 0) { perror("read"); }
    buffer[n] = '\0';
    printf("Client received: %s\n", buffer);

    close(sock);
    return 0;
}
