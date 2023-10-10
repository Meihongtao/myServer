

#include "http_conn.h"


void user_conn::init(int epollfd,int fd_,Timer *timer_,bool is_et){
    fd = fd_;
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    if(is_et){
        event.events |= EPOLLET;
        // event.events |= EPOLLONESHOT;
    }
    // 注册 以及非阻塞
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
    // 设置定时器
    timer = timer_;
    // 连接用户数
    user_count += 1;
    // 初始化http解析
    requestParser = HttpRequestParser();
    requestResponser = HttpResponse();
}

void user_conn::read()
{
    memset(readBuf,'\0',READ_BUFFER);
    while(1){
        int size = recv(this->fd,this->readBuf,this->READ_BUFFER-1,0);

        if(size <= 0){ 
            epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
            // 连接用户数
            user_count -= 1;
            std::cout << "Connection closed" << std::endl;
            break;
        }
        // else if(size < 0){
        //     if(errno == EAGAIN){
        //         epoll_event event;
        //         event.data.fd = fd;
        //         event.events = EPOLLIN | EPOLLET ;
        //         epoll_ctl(m_epollFd,EPOLL_CTL_MOD,fd,&event);
        //         break;
        //     }
        // }
        else{

                // requestParser.parse(readBuf);
                // std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
                // std::string s = "<head><body>Here is index page</body></head>";
                // requestResponser.init(200,s);
                // // std::cout << "Get data " << readBuf <<std::endl;
                // // 回显客户端发送的数据
                // send(fd, (char *)requestResponser.toString().c_str(), size, 0);
                send(fd, this->readBuf, size, 0);
                printf("get content from %d\n",fd);
                // break;
            }
            
        }
    }
    
    

void user_conn::write()
{

}
