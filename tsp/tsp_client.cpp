#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define TSP_PORT 37000
#define BUFFER_SIZE 512
#define DEFAULT_SERVER_IP "192.168.100.10"  // PC1 в Docker сети

int main(int argc, char* argv[]) {
    std::cout << "=== TSP Client (Time Stamp Protocol) ===\n";
    std::cout << "Requests time from TSP server\n\n";
    
    // Определяем IP сервера
    const char* serverIP = DEFAULT_SERVER_IP;
    if (argc > 1) {
        serverIP = argv[1];
    }
    
    int sock;
    struct sockaddr_in serverAddr;
    socklen_t addrLen = sizeof(serverAddr);
    
    // 1. Создание UDP сокета
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        return 1;
    }
    
    // 2. Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TSP_PORT);
    
    if (inet_pton(AF_INET, serverIP, &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << serverIP << std::endl;
        close(sock);
        return 1;
    }
    
    // 3. Запрос у пользователя команды
    std::cout << "TSP Server: " << serverIP << ":" << TSP_PORT << "\n";
    std::cout << "Available commands:\n";
    std::cout << "  TIME  - Local date and time\n";
    std::cout << "  UTC   - Coordinated Universal Time\n";
    std::cout << "  EPOCH - Unix timestamp (seconds since 1970)\n";
    std::cout << "  HELP  - Show this help\n";
    std::cout << "  EXIT  - Close connection (server will respond with BYE)\n\n";
    
    while (true) {
        std::cout << "Enter command (or 'quit' to exit client): ";
        std::string command;
        std::getline(std::cin, command);
        
        if (command == "quit" || command == "QUIT") {
            break;
        }
        
        if (command.empty()) {
            continue;
        }
        
        // Измеряем время отправки (для расчёта задержки)
        struct timeval startTime, endTime;
        gettimeofday(&startTime, NULL);
        
        // 4. Отправка команды серверу
        std::cout << "Sending: " << command << "\n";
        sendto(sock, command.c_str(), command.length(), 0,
               (struct sockaddr*)&serverAddr, addrLen);
        
        // 5. Ожидание ответа с таймаутом
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        
        // Установка таймаута на получение (5 секунд)
        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        
        // Получение ответа
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                     (struct sockaddr*)&serverAddr, &addrLen);
        
        // Измеряем время получения
        gettimeofday(&endTime, NULL);
        
        // Рассчитываем RTT (Round-Trip Time)
        long seconds = endTime.tv_sec - startTime.tv_sec;
        long microseconds = endTime.tv_usec - startTime.tv_usec;
        double rtt = seconds * 1000.0 + microseconds / 1000.0;  // в миллисекундах
        
        if (bytesReceived < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                std::cout << "ERROR: Timeout - no response from server\n";
            } else {
                std::cout << "ERROR: Failed to receive response\n";
            }
        } else {
            buffer[bytesReceived] = '\0';
            std::cout << "Response: " << buffer << "\n";
            std::cout << "Round-trip time: " << rtt << " ms\n";
            
            // Если сервер ответил "BYE", завершаем сессию
            if (strncmp(buffer, "BYE", 3) == 0) {
                std::cout << "Server closed the session.\n";
                break;
            }
        }
        
        std::cout << "\n";
    }
    
    std::cout << "TSP Client terminated.\n";
    close(sock);
    return 0;
}