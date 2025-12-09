#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_IP "127.255.255.255"  // Локальный broadcast (работает без прав!)
#define START_PORT 27015
#define MAX_PORTS 10  // Столько же, сколько сервер пробует
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== Broadcast Client ===\n";
    std::cout << "Using LOCAL broadcast: " << BROADCAST_IP << "\n";
    std::cout << "This works without sudo permissions!\n\n";
    
    int sock;
    struct sockaddr_in broadcastAddr;
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Включение режима BROADCAST
    int broadcastEnable = 1;
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
               &broadcastEnable, sizeof(broadcastEnable));
    
    // 3. Сообщение для отправки
    const char* message = "HELLO BROADCAST WORLD!";
    std::cout << "Message: " << message << "\n\n";
    
    // 4. Отправка на ВСЕ возможные порты
    std::cout << "Sending to ALL ports " << START_PORT << "-" 
              << (START_PORT + MAX_PORTS - 1) << ":\n";
    
    int successCount = 0;
    
    for (int port = START_PORT; port < START_PORT + MAX_PORTS; port++) {
        memset(&broadcastAddr, 0, sizeof(broadcastAddr));
        broadcastAddr.sin_family = AF_INET;
        broadcastAddr.sin_port = htons(port);
        
        // Используем inet_addr вместо inet_pton (проще)
        broadcastAddr.sin_addr.s_addr = inet_addr(BROADCAST_IP);
        
        std::cout << "  Port " << port << "... ";
        
        int bytesSent = sendto(sock, message, strlen(message), 0,
                              (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
        
        if (bytesSent < 0) {
            std::cout << "FAILED: " << strerror(errno) << "\n";
        } else {
            std::cout << "OK (" << bytesSent << " bytes)\n";
            successCount++;
        }
        
        usleep(50000);  // 50ms пауза между отправками
    }
    
    close(sock);
    
    return 0;
}