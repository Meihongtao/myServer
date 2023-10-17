#ifndef HTTPCONN_H
#define HTTPCONN_H

#include<iostream>
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<cstring>
#include "../timer/time_heap.h"
#include "http_request.h"
#include "http_response.h"


class HttpConn{
public:
    static int m_epollFd;
    static int user_count;
    static const int READ_BUFFER = 2048;
    static const int WRITE_BUFFER = 2048;
private:
    
    
    
public:
    HttpConn(){};
    void init(int fd_,Timer *timer_);
    void Close();
    // 接收请求处理并回复客户端
    void process();

    // 读取并解析http请求
    int Read(int &Error);
    // 响应资源
    void Write();   

public:

    char readBuf[READ_BUFFER];
    char writeBuf[WRITE_BUFFER];

    HttpRequestParser requestParser;
    HttpResponse requestResponser;
    Timer *timer = nullptr;
    int fd;
    
};

#endif
