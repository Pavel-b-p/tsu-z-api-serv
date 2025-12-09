#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define BROADCAST_PORT 27015           // ОДИН фиксированный порт
#define BROADCAST_IP "192.168.100.255"  // Broadcast адрес Docker сети
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== Broadcast Client ===\n";
    std::cout << "Sending to broadcast address: " << BROADCAST_IP << "\n";
    std::cout << "Port: " << BROADCAST_PORT << "\n\n";
    
    int sock;
    struct sockaddr_in broadcastAddr;
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Включение режима BROADCAST (ОБЯЗАТЕЛЬНО!)
    int broadcastEnable = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, 
                   &broadcastEnable, sizeof(broadcastEnable)) < 0) {
        perror("setsockopt SO_BROADCAST failed");
        close(sock);
        return 1;
    }
    
    // 3. Настройка broadcast адреса
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));
    broadcastAddr.sin_family = AF_INET;
    broadcastAddr.sin_port = htons(BROADCAST_PORT);  // ОДИН порт!
    
    // Преобразуем IP
    if (inet_pton(AF_INET, BROADCAST_IP, &broadcastAddr.sin_addr) <= 0) {
        std::cerr << "Invalid broadcast address: " << BROADCAST_IP << std::endl;
        close(sock);
        return 1;
    }
    
    // 4. Сообщение для отправки
    const char* message = "HELLO BROADCAST WORLD!";
    std::cout << "Message: " << message << "\n";
    std::cout << "Sending ONE broadcast packet...\n";
    
    // 5. Отправка ОДНОГО пакета на broadcast-адрес
    int bytesSent = sendto(sock, message, strlen(message), 0,
                          (struct sockaddr*)&broadcastAddr, sizeof(broadcastAddr));
    
    if (bytesSent < 0) {
        std::cout << "FAILED: " << strerror(errno) << "\n";
    } else {
        std::cout << "SUCCESS! Sent " << bytesSent << " bytes\n";
        std::cout << "Packet sent to " << BROADCAST_IP << ":" << BROADCAST_PORT << "\n";
        std::cout << "All servers in network 192.168.100.0/24 should receive this.\n";
    }
    
    close(sock);
    
    return 0;
}