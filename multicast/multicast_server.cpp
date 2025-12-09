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
    std::cout << "=== Multicast Server ===\n";
    std::cout << "Joining multicast group: " << MULTICAST_GROUP << ":" << MULTICAST_PORT << "\n\n";
    
    int sock;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in addr;
    struct ip_mreq mreq;
    socklen_t addrlen = sizeof(addr);
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Разрешаем повторное использование порта
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("setsockopt SO_REUSEADDR failed");
        close(sock);
        return 1;
    }
    
    // 3. Настройка адреса для приема
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(MULTICAST_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;  // Принимаем со всех интерфейсов
    
    // 4. Привязка сокета
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind failed");
        close(sock);
        return 1;
    }
    
    // 5. ПОДПИСКА НА MULTICAST ГРУППУ
    mreq.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    mreq.imr_interface.s_addr = INADDR_ANY;  // На всех интерфейсах
    
    if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        perror("setsockopt IP_ADD_MEMBERSHIP failed");
        close(sock);
        return 1;
    }
    
    std::cout << "✅ Successfully joined multicast group\n";
    std::cout << "Waiting for multicast messages...\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
    // 6. Основной цикл приема
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&addr, &addrlen);
        
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            char senderIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &addr.sin_addr, senderIP, INET_ADDRSTRLEN);
            
            // Проверяем, что это действительно multicast
            unsigned long addr_num = ntohl(addr.sin_addr.s_addr);
            bool is_multicast = (addr_num >= 0xE0000000 && addr_num <= 0xEFFFFFFF);
            
            std::cout << (is_multicast ? "[MULTICAST]" : "[UNICAST]");
            std::cout << " From " << senderIP << ":" << ntohs(addr.sin_port);
            std::cout << " > " << buffer << " (" << bytesReceived << " bytes)\n";
        }
    }
    
    // Отписка от группы (никогда не выполнится из-за бесконечного цикла)
    setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    close(sock);
    return 0;
}