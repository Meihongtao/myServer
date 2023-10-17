

#include "http_conn.h"


void HttpConn::init(int fd_,Timer *timer_){
    fd = fd_;
    // 设置定时器
    timer = timer_;
    // 连接用户数
    HttpConn::user_count += 1;
    // 初始化http解析
    requestParser = HttpRequestParser();
    requestResponser = HttpResponse();
}

void HttpConn::Close()
{
    HttpConn::user_count -= 1;
    close(fd);
}

void HttpConn::process()
{
    requestParser.parse(readBuf);
    std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
    std::string s = "<head><body>Here is index page</body></head>";
    requestResponser.init(200,s);
    // // std::cout << "Get data " << readBuf <<std::endl;
    // // 回显客户端发送的数据
    send(fd, (char *)requestResponser.toString().c_str(),requestResponser.toString().size(), 0);
    // send(fd, this->readBuf, size, 0);
}


int HttpConn::Read(int &errno_) {
    int totalSize = 0;
    int bytesRead = 0;
    
    while (true) {
        ssize_t bytesRead = read(fd, readBuf + totalSize, READ_BUFFER - 1 - totalSize);

        if (bytesRead > 0) {
            totalSize += bytesRead;
        } else if (bytesRead == 0) {
            // 客户端关闭连接
            errno_ = 0;
            return totalSize;
        } else if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 暂时没有更多数据可读
                errno_ = 0;
                return totalSize;
            } else {
                // 发生错误
                errno_ = errno;
                return -1;
            }
        }
    }
    return totalSize;
    
}


// int HttpConn::Read(int &Error)
// {
//     int size = 0;
//     memset(readBuf,'\0',READ_BUFFER);
//     while(1){
//         size = recv(fd,readBuf,READ_BUFFER-1,0);
//         if(size<=0){
//             if(size == 0){
//                 Error = errno;
//             }
//             break;
//         }
//         else{
//             // requestParser.parse(readBuf);
//             // std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
//             std::string s = "<head><body>Here is index page</body></head>";
//             requestResponser.init(200,s);
//             // // std::cout << "Get data " << readBuf <<std::endl;
//             // // 回显客户端发送的数据
//             send(fd, (char *)requestResponser.toString().c_str(),requestResponser.toString().size(), 0);
//             // send(fd, this->readBuf, size, 0);
//             printf("get content from %d\n",fd);
//             // break;
//         }
//     }
//     return size;
//     // while(1){
//     //     memset(readBuf,'\0',READ_BUFFER);
//     //     int size = recv(this->fd,this->readBuf,this->READ_BUFFER-1,0);

//     //     if(size <= 0){ 
//     //         if(size==0)
//     //         Error = errno;
//     //         break;
//     //     }
//     //     // else if(size < 0){
//     //     //     if(errno == EAGAIN){
//     //     //         epoll_event event;
//     //     //         event.data.fd = fd;
//     //     //         event.events = EPOLLIN | EPOLLET ;
//     //     //         epoll_ctl(m_epollFd,EPOLL_CTL_MOD,fd,&event);
//     //     //         break;
//     //     //     }
//     //     // }
//     //     else{

//     //             requestParser.parse(readBuf);
//     //             std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
//     //             std::string s = "<head><body>Here is index page</body></head>";
//     //             requestResponser.init(200,s);
//     //             // // std::cout << "Get data " << readBuf <<std::endl;
//     //             // // 回显客户端发送的数据
//     //             send(fd, (char *)requestResponser.toString().c_str(),requestResponser.toString().size(), 0);
//     //             // send(fd, this->readBuf, size, 0);
//     //             printf("get content from %d\n",fd);
//     //             // break;
//     //         }
            
//     //     }
// }
    
    

void HttpConn::Write()
{

}
