# Dockerfile
FROM ubuntu:22.04

# Устанавливаем зависимости
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    net-tools \
    iproute2 \
    iputils-ping \
    tcpdump \
    && rm -rf /var/lib/apt/lists/*

# Копируем весь проект
WORKDIR /app
COPY . .

# Собираем проект
RUN make clean && make

# По умолчанию запускаем главное меню
CMD ["./main"]