#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <queue>
#include <chrono>
#include <thread>
#include <memory>
#include <fcntl.h>

#include "../pool/threadsPool.h"
#include "../http/http_conn.h"
// #include "../timer/time_heap.h"


#define IS_ET true

const int MAX_EVENTS = 100;
const int BUFFER_SIZE = 1024;



extern int user_conn::m_epollFd = -1;

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // 创建监听套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // 绑定监听套接字到本地地址和端口
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12346);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return 1;
    }

    // 开始监听连接
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return 1;
    }

    // 创建 epoll 实例
    int epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Failed to create epoll instance" << std::endl;
        return 1;
    }

    user_conn::m_epollFd = epollFd;

    // 添加监听套接字到 epoll 实例中
    struct epoll_event event;
    event.data.fd = serverSocket;
    event.events = EPOLLIN;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        std::cerr << "Failed to add server socket to epoll" << std::endl;
        return 1;
    }

    // 创建小根堆（模拟）
    std::priority_queue<Timer*, std::vector<Timer*>, std::greater<Timer*>> timerQueue;
    auto time_out = 0;
    char buffer[BUFFER_SIZE];
    user_conn *users =new user_conn[MAX_EVENTS];


    while (true) {
        // 检查定时器队列，关闭超时连接
        while (!timerQueue.empty()) {
            Timer *timer = timerQueue.top();
            if (std::chrono::steady_clock::now() >= timer->expiry) {
                int close_fd = timer->fd;
                epoll_ctl(epollFd, EPOLL_CTL_DEL, close_fd, nullptr);
                close(close_fd);
                std::cout << "Connection timed out and closed" << std::endl;
                timerQueue.pop();
                
            } else {
                auto time_out = timer->expiry - std::chrono::steady_clock::now();
                break; // 因为小根堆中的定时器是按时间排序的，所以可以提前退出循环
            }
        }


        struct epoll_event events[MAX_EVENTS];
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, time_out);

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverSocket) {
                // 新的客户端连接请求
                int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
                Timer *timer =new Timer();
                timer->expiry = std::chrono::steady_clock::now() + std::chrono::seconds(5); // 60秒超时
                timerQueue.push(timer);
                (users[clientSocket]).init(epollFd,clientSocket,timer,IS_ET);
                timer->fd = clientSocket;
                std::cout << "Accepted new connection from "
                          << inet_ntoa(clientAddr.sin_addr)
                          << ":" << ntohs(clientAddr.sin_port) << std::endl;

                // 创建并添加定时器，模拟超时关闭
                
            } else if(events[i].events & EPOLLIN) {
                // 客户端有数据可读
                int fd_ = events[i].data.fd;
                users[fd_].timer->expiry = std::chrono::steady_clock::now() + std::chrono::seconds(5);
                users[fd_].read();
                // int bytesRead = recv(fd_, buffer, BUFFER_SIZE, 0);

                // if (bytesRead <= 0) {
                //     // 客户端关闭连接
                //     epoll_ctl(epollFd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                //     close(events[i].data.fd);
                //     std::cout << "Connection closed" << std::endl;
                // } else {
                //     // 回显客户端发送的数据
                //     send(events[i].data.fd, buffer, bytesRead, 0);
                // }
            }
        }

        
    }

    // 关闭监听套接字和 epoll 实例
    close(serverSocket);
    close(epollFd);

    return 0;
}
