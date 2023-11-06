#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <cstring>

#include "http_request.h"
#include "http_response.h"
#include "../timer/time_heap.h"
#include "../log/log.h"

class HttpConn
{
public:
    static int m_epollFd;
    static int user_count;
    static const int READ_BUFFER = 2048;
    static const int WRITE_BUFFER = 2048;

private:
public:
    HttpConn(){};
    ~HttpConn();
    void init(int fd_, TimerNode *timer_);
    void Close();
    // 接收请求处理并回复客户端
    bool process();

    // 读取并解析http请求
    int Read(int &readError);
    // 响应资源
    int Write(int &writeError);
    bool isKeepAlive() const;
    int toWriteBytes();
    int totalSize;

public:
    char readBuf[READ_BUFFER];
    std::string writeBuf;
    int iov_count;
    // ivo[0]头部内容 ivo[1] 文件内容
    struct iovec iov[2];
    HttpRequestParser requestParser;
    HttpResponse requestResponser;
    TimerNode *timer = nullptr;
    bool isClose = false;
    int fd;
    
};

#endif
