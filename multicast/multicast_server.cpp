#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>

#define START_PORT 28000  
#define MAX_PORT_ATTEMPTS 10
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== Multicast Server (Local Emulation) ===\n";
    std::cout << "Emulating multicast subscription\n\n";
    
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
    
    // 2. Пытаемся привязаться к порту
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
            std::cout << "busy\n";
            currentPort++;
        }
    }
    
    if (!portBound) {
        std::cerr << "ERROR: No free ports\n";
        close(sock);
        return 1;
    }
    
    std::cout << "\n📡 Server subscribed to 'multicast group' (port " << currentPort << ")\n";
    std::cout << "Waiting for multicast messages...\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    // 3. Основной цикл
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&clientAddr, &clientLen);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
            
            std::cout << "[" << currentPort << "] Multicast received: "
                     << buffer << " (" << bytesReceived << " bytes)\n";
        }
    }
    
    close(sock);
    return 0;
}