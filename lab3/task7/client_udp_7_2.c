// Цель: Демонстрация локального UDP/IP клиента
//       Отправляет датаграмму на 127.0.0.1:5001 и получает ответ от сервера.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5001
#define BUF_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t serv_len = sizeof(serv_addr);
    char buffer[BUF_SIZE];

    // 1. Создать UDP-сокет
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    // 2. Настроить адрес сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton"); exit(1);
    }

    // 3. Send message
    const char *msg = "Hi from UDP client, new client!";
    sendto(sockfd, msg, strlen(msg), 0,
           (struct sockaddr*)&serv_addr, serv_len);
    printf("Client sent: %s\n", msg);

    // 4. Receive response
    int n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                     NULL, NULL);
    if (n < 0) { perror("recvfrom"); exit(1); }
    buffer[n] = '\0';
    printf("Client received: %s\n", buffer);

    close(sockfd);
    return 0;
}
