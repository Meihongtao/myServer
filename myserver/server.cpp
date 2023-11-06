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
#define TIMEOUT_MS 10000


bool myServer::initListen()
{
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    // 创建监听套接字
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // 绑定监听套接字到本地地址和端口
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(12349);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int enable_reuse = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable_reuse, sizeof(int));

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Failed to bind socket" << std::endl;
        return false;
    }

    // 开始监听连接
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Failed to listen on socket" << std::endl;
        return false;
    }
    // ET 模式
    epoller->addFd(serverSocket, EPOLLIN);
    fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);
}

void myServer::start()
{
    int timeMs = -1;
    while (1)
    {
        
        timeMs = heapTimer->GetNextTick();
        
        
        int num = epoller->Wait(timeMs);
        if (num == -1)
        {
            printf("epoll wait error!\n");
            break;
        }
        // printf("num = %d\n",num);
        for (int i = 0; i < num; i++)
        {
            int fd = epoller->getFd(i);
            u_int32_t event = epoller->getEvent(i);
            if (fd == serverSocket)
            {
                connHandler();
            }
            else if (event & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
            {
                // close fd
                // printf("in EPOLLRDHUP\n");
                closeHandler(users[fd]);
            }
            else if (event & EPOLLIN)
            {
                // readHandler(users[fd]);
                pool->addTask([this, fd]()
                              { readHandler(users[fd]); });
            }
            else if (event & EPOLLOUT)
            {
                // writeHandler(users[fd]);
                // write fd
                // writeHandler(users[fd]);
                pool->addTask([this, fd]()
                              { writeHandler(users[fd]); });
            }
        }
    }
}

void myServer::extentTime(HttpConn &httpconn)
{
    if(TIMEOUT_MS > 0) { heapTimer->adjust(httpconn.fd, TIMEOUT_MS);}
}

void myServer::Process(HttpConn &httpconn)
{
    if (httpconn.process())
    {
        epoller->modFd(httpconn.fd, EPOLLET | EPOLLONESHOT | EPOLLOUT);
    }
    else
    {
        epoller->modFd(httpconn.fd, EPOLLET | EPOLLONESHOT | EPOLLIN);
    }
    // closeHandler(httpconn);
}

void myServer::readHandler(HttpConn &httpconn)
{
    int size = 0, errno_ = 0;
    assert(httpconn.fd > 0);
    extentTime(httpconn);
    size = httpconn.Read(errno_);
    if (size < 0)
    {
        if (errno != EAGAIN)
        {
            // 出现错误
            closeHandler(httpconn);
            return;
            // epoller->modFd(httpconn.fd, EPOLLOUT | EPOLLET | EPOLLONESHOT);
            // printf("read later %d\n",httpconn.fd);
        }
        else if (size == 0)
        {
            // 客户端关闭连接，需要关闭套接字
            // printf("close fd %d size=0\n",httpconn.fd);
            closeHandler(httpconn);
            return;
        }
    }
    Process(httpconn);
}

void myServer::connHandler()
{
    do
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0)
        {
            break;
        }
        // if(clientSocket >= 10000)
        if(clientSocket >= 10000){
            AsyncLogger::getInstance().log(LogLevel::WARNING,clientSocket + "connection refused !");
            continue;
        }else{
            users[clientSocket].init(clientSocket,nullptr);  
            if (epoller->addFd(clientSocket, EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLONESHOT) == false)
            {
                // std::cout << strerror(errno) << std::endl;
                // AsyncLogger::getInstance().log(LogLevel::ERROR,strerror(errno));
            }
            else{
                // std::stringstream ss;
                // ss << "new connection " << clientSocket;
                // AsyncLogger::getInstance().log(LogLevel::INFO,ss.str());
                printf("add timer for fd %d\n",clientSocket);
                heapTimer->add(clientSocket,TIMEOUT_MS,std::bind(&myServer::closeHandler,this,users[clientSocket]));
            }
            // 非阻塞
            fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);
        }
        
        // printf("new conn %d\n",clientSocket);
    } while (true);
}

void myServer::writeHandler(HttpConn &httpconn)
{
    int ret = -1;
    int writeErrno = 0;
    extentTime(httpconn);
    // printf("in write handler\n");
    ret = httpconn.Write(writeErrno);
    if (httpconn.toWriteBytes() == 0)
    {
        /* 传输完成 */
        closeHandler(httpconn);
        return;
        // if(httpconn.isKeepAlive()) {
        //     Process(httpconn);
        //     return;
        // }
    }
    else if (ret < 0)
    {
        if (writeErrno == EAGAIN)
        {
            /* 继续传输 */
            // printf("write eagain\n");
            epoller->modFd(httpconn.fd, EPOLLET | EPOLLONESHOT | EPOLLOUT);
            return;
        }
    }
    closeHandler(httpconn);
}

void myServer::closeHandler(HttpConn &httpconn)
{

    if(httpconn.isClose == false){
        if (epoller->delFd(httpconn.fd) == false)
        {
            std::cout << "close errno " << strerror(errno) << std::endl;
            AsyncLogger::getInstance().log(LogLevel::ERROR,"close httpconn error!");
            return;
        }
    } 
    printf("fd %d ready close\n",httpconn.fd);
    httpconn.Close();
    // printf("close fd %d\n",httpconn.fd);
}

myServer::~myServer()
{
    
    delete [] users;
    SqlPool::getInstance()->closePool();
    AsyncLogger::getInstance().stopLogging();
}


int main()
{
    myServer server(12349, 8, 0, 3306, "root", "12345678", "mydb", 12);
    char buffer[2048]; // 用于存储目录路径的缓冲区，PATH_MAX 是一个宏，定义了最大路径长度
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        std::cout << "当前工作目录: " << buffer << std::endl;
    }
    else
    {
        std::cerr << "无法获取当前工作目录." << std::endl;
    }
    server.start();
}
