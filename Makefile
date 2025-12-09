CC = g++
CFLAGS = -std=c++11 -Wall -Wextra

# Создаем папки если их нет
DIRS = broadcast multicast udp

# Все цели
TARGETS = main broadcast/broadcast_server broadcast/broadcast_client multicast/multicast_server multicast/multicast_client udp/udp_server udp/udp_client

all: create_dirs $(TARGETS)

# Создание директорий
create_dirs:
	@mkdir -p $(DIRS)

# Главная программа
main: main.cpp
	$(CC) $(CFLAGS) -o main main.cpp

# 1. Обычный UDP сервер
udp/udp_server: udp/udp_server.cpp
	$(CC) $(CFLAGS) -o udp/udp_server udp/udp_server.cpp

# 2. Обычный UDP клиент
udp/udp_client: udp/udp_client.cpp
	$(CC) $(CFLAGS) -o udp/udp_client udp/udp_client.cpp

# 3. Broadcast сервер
broadcast/broadcast_server: broadcast/broadcast_server.cpp
	$(CC) $(CFLAGS) -o broadcast/broadcast_server broadcast/broadcast_server.cpp

# 4. Broadcast клиент
broadcast/broadcast_client: broadcast/broadcast_client.cpp
	$(CC) $(CFLAGS) -o broadcast/broadcast_client broadcast/broadcast_client.cpp

# 5. Multicast сервер
multicast/multicast_server: multicast/multicast_server.cpp
	$(CC) $(CFLAGS) -o multicast/multicast_server multicast/multicast_server.cpp

# 6. Multicast клиент
multicast/multicast_client: multicast/multicast_client.cpp
	$(CC) $(CFLAGS) -o multicast/multicast_client multicast/multicast_client.cpp

# Очистка
clean:
	rm -f main
	rm -f broadcast/broadcast_server broadcast/broadcast_client
	rm -f multicast/multicast_server multicast/multicast_client
	rm -f udp/udp_server udp/udp_client
	@if [ -d broadcast ] && [ -z "$$(ls -A broadcast)" ]; then rmdir broadcast; fi
	@if [ -d multicast ] && [ -z "$$(ls -A multicast)" ]; then rmdir multicast; fi
	@if [ -d udp ] && [ -z "$$(ls -A udp)" ]; then rmdir udp; fi

# Запуск главной программы
run: main
	./main