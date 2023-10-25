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
#include "server.h"
// #include "../timer/time_heap.h"


#define IS_ET true
#define TIMEOUT 60


bool myServer::initListen()
{
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // 创建监听套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // 绑定监听套接字到本地地址和端口
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12349);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }

    // 开始监听连接
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }
    // ET 模式
    epoller->addFd(serverSocket,EPOLLIN);
    fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);
}

void myServer::start()
{
    while(1){
        int num = epoller->Wait(-1);
        if(num == -1){
            printf("epoll wait error!\n");
            break;
        }
        // printf("num = %d\n",num);
        for(int i=0;i<num;i++){
            int fd = epoller->getFd(i);
            u_int32_t event = epoller->getEvent(i);
            if(fd == serverSocket){
                connHandler();
            }
            else if(event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                // close fd
                // printf("in EPOLLRDHUP\n");
                closeHandler(users[fd]);
            }
            else if(event & EPOLLIN){
                // readHandler(users[fd]);
                pool->addTask([this,fd](){
                   readHandler(users[fd]);
                });
            }
            else if(event & EPOLLOUT){
                // writeHandler(users[fd]);
                // write fd
                // writeHandler(users[fd]);
                pool->addTask([this,fd](){
                    writeHandler(users[fd]);
                });
               
            }
        }
    }
}

void myServer::readHandler(HttpConn& httpconn)
{
    int size = 0,errno_ = 0;
    assert(httpconn.fd >0);
    size = httpconn.Read(errno_);
    if (size < 0) {
    if (errno == EAGAIN) {
        // 暂时没有数据可读，稍后再尝试
        epoller->modFd(httpconn.fd, EPOLLOUT | EPOLLET | EPOLLONESHOT);
        // printf("read later %d\n",httpconn.fd);
    } else {
        // 其他错误，需要关闭套接字
        // printf("close fd %d other error\n",httpconn.fd);
        closeHandler(httpconn);
    }
    } else if (size == 0) {
        // 客户端关闭连接，需要关闭套接字
        // printf("close fd %d size=0\n",httpconn.fd);
        closeHandler(httpconn);
    } 
    
    
   
    
    
    // while(1){
    //     size = recv(httpconn.fd,httpconn.readBuf+total_size,HttpConn::READ_BUFFER-1,0);
    //     if(size<=0){
    //         if (size==0){
    //             closeHandler(httpconn);
    //         }
    //         else{
    //             if (errno == EAGAIN || errno == EWOULDBLOCK) {
    //             printf("read later\n");
    //             // 这些错误表示当前没有数据可读，可以继续等待
    //             closeHandler(httpconn); // 关闭连接
               
    //             // sleep(1);
    //             } else {
    //                 // 处理其他错误，可能需要关闭连接
    //                 perror("recv error");
    //                 closeHandler(httpconn); // 关闭连接
    //             }
    //         }
    //         break;
    //     }
    //     else{
    //         total_size += size;
    //         // requestParser.parse(readBuf);
    //         // std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
    //         std::string s = "<head><body>Here is index page</body></head>";
    //         httpconn.requestResponser.init(200,s);
    //         // // std::cout << "Get data " << readBuf <<std::endl;
    //         // // 回显客户端发送的数据
    //         send(httpconn.fd, (char *)httpconn.requestResponser.toString().c_str(),httpconn.requestResponser.toString().size(), 0);
    //         // send(fd, this->readBuf, size, 0);
    //         printf("get content %d bytes from %d\n",size,httpconn.fd);
    //         // break;
    //     }
    // }
}

void myServer::connHandler()
{
    do{
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0){
            break;
        }
        users[clientSocket].init(clientSocket,nullptr);
        if(epoller->addFd(clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLONESHOT )==false){
            std::cout << strerror(errno) << std::endl;
        }
        // 非阻塞
        fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
        // printf("new conn %d\n",clientSocket);
    }while(true);
}

void myServer::writeHandler(HttpConn &httpconn)
{
    httpconn.process();
    // epoller->modFd(httpconn.fd,EPOLLIN | EPOLLET);
    closeHandler(httpconn);
}

void myServer::closeHandler(HttpConn& httpconn)
{
    
    if(epoller->delFd(httpconn.fd)==false){
         std::cout << "close errno " << strerror(errno) << std::endl;
    }
    httpconn.Close();
    // printf("close fd %d\n",httpconn.fd);
}

int main(){
    myServer server = myServer(12349,8,0);
    char buffer[2048]; // 用于存储目录路径的缓冲区，PATH_MAX 是一个宏，定义了最大路径长度
    if (getcwd(buffer, sizeof(buffer)) != NULL) {
        std::cout << "当前工作目录: " << buffer << std::endl;
    } else {
        std::cerr << "无法获取当前工作目录." << std::endl;
    }
    server.start();
}
