#include <iostream>
#include <cstdlib>
#include <string>

int main() {
    std::cout << "=== Сетевое программирование: UDP/Broadcast/Multicast ===\n";
    std::cout << "Выберите программу для запуска:\n";
    std::cout << "1. Обычный UDP сервер (Echo)\n";
    std::cout << "2. Обычный UDP клиент\n";
    std::cout << "3. Broadcast сервер (приём сообщений)\n";
    std::cout << "4. Broadcast клиент (отправка сообщений)\n";
    std::cout << "5. Multicast сервер\n";
    std::cout << "6. Multicast клиент\n";
    std::cout << "7. TSP сервер (Time Stamp Protocol)\n";
    std::cout << "8. TSP клиент\n";
    std::cout << "0. Выход\n";
    std::cout << "Выбор: ";
    
    int choice;
    std::cin >> choice;
    
    // Очистка буфера ввода
    std::cin.ignore(100, '\n');
    
    switch(choice) {
        case 0:
            std::cout << "Выход...\n";
            return 0;
            
        case 1:
            std::cout << "Запуск UDP сервера...\n";
            system("./udp/udp_server");
            break;
            
        case 2:
            std::cout << "Запуск UDP клиента...\n";
            system("./udp/udp_client");
            break;
            
        case 3:
            std::cout << "Запуск Broadcast сервера...\n";
            system("./broadcast/broadcast_server");
            break;
            
        case 4:
            std::cout << "Запуск Broadcast клиента...\n";
            system("./broadcast/broadcast_client");
            break;
            
        case 5:
            std::cout << "Запуск Multicast сервера...\n";
            system("./multicast/multicast_server");
            break;
            
        case 6:
            std::cout << "Запуск Multicast клиента...\n";
            system("./multicast/multicast_client");
            break;
        
        case 7:
            std::cout << "Запуск TSP сервера...\n";
            system("./tsp/tsp_server");
            break;
    
        case 8:
            std::cout << "Запуск TSP клиента...\n";
            system("./tsp/tsp_client");
            break;

            
        default:
            std::cout << "Неверный выбор!\n";
    }
    
    return 0;
}