#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define START_PORT 27015
#define MAX_PORT_ATTEMPTS 10  // Пробуем 10 портов
#define BUFFER_SIZE 512

int main() {
    int sock;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientLen = sizeof(clientAddr);
    
    int currentPort = START_PORT;
    bool portBound = false;
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. УБИРАЕМ SO_REUSEADDR - он позволяет нескольким процессам 
    //    слушать один порт, что нам не нужно
    
    // 3. Пытаемся привязаться к порту, начиная со START_PORT
    for (int attempt = 0; attempt < MAX_PORT_ATTEMPTS; attempt++) {
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(currentPort);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        
        std::cout << "Trying port " << currentPort << "... ";
        
        if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == 0) {
            portBound = true;
            std::cout << "SUCCESS!\n";
            break;
        } else {
            std::cout << "busy (error: " << strerror(errno) << ")\n";
            currentPort++;  // Пробуем следующий порт
        }
    }
    
    if (!portBound) {
        std::cerr << "ERROR: Could not bind to any port in range "
                  << START_PORT << "-" << (START_PORT + MAX_PORT_ATTEMPTS - 1) << "\n";
        close(sock);
        return 1;
    }
    
    std::cout << "\n=== Broadcast Server ===\n";
    std::cout << "Listening on port: " << currentPort << "\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    // 4. Основной цикл приема broadcast-сообщений
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&clientAddr, &clientLen);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            
            std::cout << "[" << currentPort << "] Received from " << clientIP << ": "
                     << buffer << " (" << bytesReceived << " bytes)\n";
        }
    }
    
    close(sock);
    return 0;
}