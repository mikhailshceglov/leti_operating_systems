// Цель: Демонстрация локального TCP/IP сервера
//       Создаёт сокет, bind() на порт 5000, слушает и принимает одно соединение,
//       читает строку от клиента и отправляет ответ.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5000
#define BUF_SIZE 1024

int main() {
    int server_fd, client_fd;
    struct sockaddr_in addr;
    char buffer[BUF_SIZE];
    
    // 1. Создать TCP-сокет
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); exit(1); }

    // 2. Настроить адрес
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    // 3. Bind и Listen
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); exit(1);
    }
    if (listen(server_fd, 1) < 0) {
        perror("listen"); exit(1);
    }
    printf("TCP server listening on port %d...\n", PORT);

    // 4. Accept
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd < 0) { perror("accept"); exit(1); }
    printf("Client connected.\n");

    // 5. Получить сообщение от клиента
    int n = read(client_fd, buffer, BUF_SIZE - 1);
    if (n < 0) { perror("read"); }
    buffer[n] = '\0';
    printf("Server received: %s\n", buffer);

    // 6. Отправить ответ
    const char *resp = "This is from TCP server!!";
    write(client_fd, resp, strlen(resp));
    printf("Server finished.\n");

    close(client_fd);
    close(server_fd);
    return 0;
}
