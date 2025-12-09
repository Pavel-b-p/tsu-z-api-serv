#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 27015  // Тот же порт, что у клиента
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
    
    // 2. Разрешаем повторное использование порта
    int reuseAddr = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(reuseAddr));
    
    // 3. Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(BROADCAST_PORT);  // Тот же порт 27015!
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Слушаем на всех интерфейсах
    
    // 4. Привязка к порту
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        close(sock);
        return 1;
    }
    
    std::cout << "=== Broadcast Server ===\n";
    std::cout << "Listening for broadcast on port " << BROADCAST_PORT << "\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    // 5. Основной цикл приема
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&clientAddr, &clientLen);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            
            std::cout << "Received broadcast from " << clientIP << ":" 
                     << ntohs(clientAddr.sin_port) << " - " 
                     << buffer << " (" << bytesReceived << " bytes)\n";
        }
    }
    
    close(sock);
    return 0;
}