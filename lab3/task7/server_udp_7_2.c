// Цель: Демонстрация локального UDP/IP сервера
//       Создаёт сокет, bind() на порт 5001, получает одно датаграммное сообщение от клиента
//       и отправляет ответ тем же клиентом.

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
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    char buffer[BUF_SIZE];

    // 1. Создать UDP-сокет
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) { perror("socket"); exit(1); }

    // 2. Настроить адрес сервера
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // 3. Bind
    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("bind"); exit(1);
    }
    printf("UDP server listening on port %d...\n", PORT);

    // 4. Receive message
    int n = recvfrom(sockfd, buffer, BUF_SIZE - 1, 0,
                     (struct sockaddr*)&cli_addr, &cli_len);
    if (n < 0) { perror("recvfrom"); exit(1); }
    buffer[n] = '\0';
    printf("Server received from %s:%d: %s\n",
           inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);

    // 5. Send response
    const char *resp = "Hi from UDP server, new server!!";
    sendto(sockfd, resp, strlen(resp), 0,
           (struct sockaddr*)&cli_addr, cli_len);
    printf("Server sent response.\n");

    close(sockfd);
    return 0;
}
