#include<iostream>
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/socket.h>
#include<cstring>
#include "../timer/time_heap.h"


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
    
    void process();

    void read();

    void write();   

public:

    char readBuf[READ_BUFFER];
    char writeBuf[WRITE_BUFFER];

    Timer *timer = nullptr;
    int fd;
    
};

