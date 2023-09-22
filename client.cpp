#include <iostream>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 创建套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return -1;
    }

    // 设置服务器地址和端口
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(12346);
    if (inet_pton(AF_INET, "127.0.0.1", &(serverAddress.sin_addr)) <= 0) {
        std::cerr << "Invalid address/Address not supported" << std::endl;
        return -1;
    }

    // 连接到服务器
    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Failed to connect to server" << std::endl;
        return -1;
    }

    // 每10秒发送一条消息
    while (true) {
        std::string message = "Hello, server!";
        if (send(clientSocket, message.c_str(), message.size(), 0) == -1) {
            std::cerr << "Failed to send message" << std::endl;
            break;
        }

        std::cout << "Message sent: " << message << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(20));
    }

    // 关闭套接字
    close(clientSocket);

    return 0;
}