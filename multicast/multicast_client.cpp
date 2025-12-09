#include <iostream>
#include <cstring>
#include <vector>
#include <unistd.h>
#include <arpa/inet.h>

#define START_PORT 28000  // Начинаем с 28000
#define GROUP_SIZE 2      // Сколько "подписчиков" в группе
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== Multicast Client (Local Emulation) ===\n";
    std::cout << "Sending to 'multicast group' (emulated)\n\n";
    
    // "Группа подписчиков" - список портов
    std::vector<int> groupPorts;
    for (int i = 0; i < GROUP_SIZE; i++) {
        groupPorts.push_back(START_PORT + i);
    }
    
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    const char* message = "MULTICAST MESSAGE TO GROUP!";
    std::cout << "Message: " << message << "\n";
    std::cout << "Group ports: ";
    for (int port : groupPorts) std::cout << port << " ";
    std::cout << "\n\n";
    
    std::cout << "Emulating multicast to group...\n";
    std::cout << "----------------------------------------\n";
    
    int successCount = 0;
    
    // Отправляем каждому "подписчику" в группе
    for (int port : groupPorts) {
        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1
        
        std::cout << "Port " << port << "... ";
        
        int bytesSent = sendto(sock, message, strlen(message), 0,
                              (sockaddr*)&addr, sizeof(addr));
        
        if (bytesSent > 0) {
            std::cout << "SENT (" << bytesSent << " bytes)\n";
            successCount++;
        } else {
            std::cout << "FAILED: " << strerror(errno) << "\n";
        }
        
        usleep(100000);  // 100ms
    }
    
    close(sock);
    
    std::cout << "----------------------------------------\n";
    std::cout << "Sent to " << successCount << "/" << GROUP_SIZE 
              << " group members\n\n";

    return 0;
}