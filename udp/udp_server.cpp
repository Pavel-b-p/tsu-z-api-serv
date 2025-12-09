#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 27015
#define BUFFER_SIZE 512

int main() {
    int sock;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Слушаем на всех интерфейсах
    
    // 3. Привязка сокета к порту
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        close(sock);
        return 1;
    }
    
    std::cout << "UDP Server listening on port " << SERVER_PORT << "\n";
    std::cout << "Press Ctrl+C to stop (will quit with error)\n";
    
    // 4. Основной цикл приема
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // Получение данных от клиента
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&clientAddr, &clientLen);
        
        if (bytesReceived < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        buffer[bytesReceived] = '\0';
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        
        std::cout << "Received from " << clientIP << ":" << ntohs(clientAddr.sin_port)
                  << ": " << buffer << "\n";
        
        // ОПЦИОНАЛЬНО: отправляем echo (можно убрать, если не нужно)
        sendto(sock, buffer, bytesReceived, 0,
               (struct sockaddr*)&clientAddr, clientLen);
    }
    
    close(sock);
    return 0;
}