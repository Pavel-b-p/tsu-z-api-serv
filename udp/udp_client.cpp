#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_PORT 27115
#define SERVER_IP "192.168.100.10"  // PC1 (сервер) в Docker сети
#define BUFFER_SIZE 512

int main() {
    int sock;
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
    serverAddr.sin_port = htons(SERVER_PORT);
    
    // Используем твой реальный IP
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << SERVER_IP << std::endl;
        close(sock);
        return 1;
    }
    
    // 3. Сообщение для отправки
    const char* message = "Hello UDP Server!";
    
    // 4. ОТПРАВЛЯЕМ И ЗАБЫВАЕМ - ЭТО UDP, БЛЯДЬ!
    std::cout << "Sending UDP datagram to " << SERVER_IP << ":" << SERVER_PORT << "\n";
    std::cout << "Message: " << message << "\n";
    
    sendto(sock, message, strlen(message), 0,
           (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    
    std::cout << "Datagram sent. No response expected (UDP is connectionless).\n";
    
    close(sock);
    return 0;
}