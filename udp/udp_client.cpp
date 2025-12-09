#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 27015
#define BUFFER_SIZE 512
#define SERVER_IP "127.0.0.1"  // Локальный хост

int main() {
    int sock;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddr;
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
    
    // 3. Сообщение для отправки
    const char* message = "Hello from UDP client!";
    std::cout << "Sending: " << message << "\n";
    
    // 4. Отправка сообщения на сервер
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    // 5. Ожидание ECHO-ответа
    std::cout << "Waiting for echo response...\n";
    
    socklen_t serverLen = sizeof(serverAddr);
    int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr*)&serverAddr, &serverLen);
    
    if (bytesReceived > 0) {
        buffer[bytesReceived] = '\0';
        std::cout << "Received from server: " << buffer << "\n";
    } else {
        std::cout << "No response received\n";
    }
    
    close(sock);
    return 0;
}