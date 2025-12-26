#include <iostream>
#include <cstring>
#include <ctime>
#include <unistd.h>
#include <arpa/inet.h>

#define TSP_PORT 37000
#define BUFFER_SIZE 512

int main() {
    std::cout << "=== TSP Server (Time Stamp Protocol) ===\n";
    std::cout << "Listening on port " << TSP_PORT << "\n";
    std::cout << "Responds to 'TIME' requests with current timestamp\n";
    std::cout << "Press Ctrl+C to stop...\n\n";
    
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
    
    // 2. Настройка адреса сервера
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TSP_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    
    // 3. Привязка сокета к порту
    if (bind(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind failed");
        close(sock);
        return 1;
    }
    
    // 4. Основной цикл приема
    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // Получение запроса от клиента
        int bytesReceived = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                                   (struct sockaddr*)&clientAddr, &clientLen);
        
        if (bytesReceived < 0) {
            perror("recvfrom failed");
            continue;
        }
        
        buffer[bytesReceived] = '\0';
        char clientIP[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
        
        std::cout << "Received from " << clientIP << ":" << ntohs(clientAddr.sin_port)
                  << " > " << buffer << "\n";
        
        // 5. Проверка запроса и формирование ответа
        std::string response;
        std::string request(buffer);
        
        if (request == "TIME" || request == "time") {
            // Получаем текущее время
            time_t rawtime;
            struct tm * timeinfo;
            char timeBuffer[80];
            
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S", timeinfo);
            
            // Формат ответа: "TIMESTAMP <время>"
            response = "TIMESTAMP ";
            response += timeBuffer;
        } else if (request == "UTC" || request == "utc") {
            // Время в UTC
            time_t rawtime;
            struct tm * timeinfo;
            char timeBuffer[80];
            
            time(&rawtime);
            timeinfo = gmtime(&rawtime);
            strftime(timeBuffer, sizeof(timeBuffer), "%Y-%m-%d %H:%M:%S UTC", timeinfo);
            
            response = "UTC ";
            response += timeBuffer;
        } else if (request == "EPOCH" || request == "epoch") {
            // Unix timestamp (секунды с 1970)
            time_t epoch = time(nullptr);
            response = "EPOCH " + std::to_string(epoch);
        } else if (request == "HELP" || request == "help") {
            // Справка по командам
            response = "Available commands: TIME, UTC, EPOCH, HELP, EXIT";
        } else if (request == "EXIT" || request == "exit") {
            response = "BYE";
        } else {
            response = "ERROR: Unknown command. Send 'HELP' for list of commands.";
        }
        
        // 6. Отправка ответа клиенту
        sendto(sock, response.c_str(), response.length(), 0,
               (struct sockaddr*)&clientAddr, clientLen);
        
        std::cout << "Sent response: " << response << "\n\n";
        
        // Если клиент запросил EXIT, ждём следующего клиента
    }
    
    close(sock);
    return 0;
}