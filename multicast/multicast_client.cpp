#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MULTICAST_PORT 28000
#define MULTICAST_GROUP "239.255.0.1"  // Multicast группа
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== Multicast Client ===\n";
    std::cout << "Sending to multicast group: " << MULTICAST_GROUP << ":" << MULTICAST_PORT << "\n\n";
    
    int sock;
    struct sockaddr_in addr;
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Устанавливаем TTL (Time To Live) для multicast
    // TTL = сколько маршрутизаторов может пройти пакет
    unsigned char ttl = 1;  // Только локальная сеть
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
        perror("setsockopt IP_MULTICAST_TTL failed");
        close(sock);
        return 1;
    }
    
    // 3. Отключаем loopback (чтобы сам отправитель не получал свои пакеты)
    unsigned char loop = 0;  // 0 = отключить loopback
    if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0) {
        perror("setsockopt IP_MULTICAST_LOOP failed");
        close(sock);
        return 1;
    }
    
    // 4. Настройка multicast адреса для отправки
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MULTICAST_PORT);
    
    if (inet_pton(AF_INET, MULTICAST_GROUP, &addr.sin_addr) <= 0) {
        std::cerr << "Invalid multicast address: " << MULTICAST_GROUP << std::endl;
        close(sock);
        return 1;
    }
    
    // 5. Сообщение для отправки
    const char* message = "MULTICAST MESSAGE TO GROUP!";
    std::cout << "Message: " << message << "\n";
    std::cout << "Sending ONE packet to multicast group...\n";
    
    // 6. Отправка ОДНОГО пакета на multicast группу
    int bytesSent = sendto(sock, message, strlen(message), 0,
                          (struct sockaddr*)&addr, sizeof(addr));
    
    if (bytesSent < 0) {
        std::cout << "FAILED: " << strerror(errno) << "\n";
        
        if (errno == EACCES) {
            std::cout << "\nNOTE: Multicast may require:\n";
            std::cout << "1. Proper network configuration\n";
            std::cout << "2. Firewall rules for multicast\n";
        }
    } else {
        std::cout << "✅ SUCCESS! Sent " << bytesSent << " bytes\n";
        std::cout << "All servers in multicast group " << MULTICAST_GROUP;
        std::cout << " should receive this packet.\n";
    }
    
    close(sock);
    return 0;
}