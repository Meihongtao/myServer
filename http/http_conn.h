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

class user_conn{
public:
    static int m_epollFd;
    static int user_count;
private:
    static const int READ_BUFFER = 2048;
    static const int WRITE_BUFFER = 2048;
    
public:
    user_conn(){};
    void init(int epollfd,int fd_,Timer *timer_,bool is_et);
    
    // 接收请求处理并回复客户端
    void process();

    // 读取并解析http请求
    void read();
    // 响应资源
    void write();   

public:

    char readBuf[READ_BUFFER];
    char writeBuf[WRITE_BUFFER];

    Timer *timer = nullptr;
    int fd;
    
};

#endif
