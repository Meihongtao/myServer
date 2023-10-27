

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
    std::string message = "success";
    std::string resource= "/";
    int statuscode = 200;
    
    
    requestParser.parse(readBuf);
    // std::cout << readBuf << std::endl;
    std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
    // std::string s = "<head><body>Here is index page</body></head>";
    // requestResponser.init(200,s);
    resource = requestParser.getResource();
    if(requestParser.getMethod() == "POST"){
        bool ret = requestParser.postHandler();
        if(ret){
            message = "success";
        }
        else{
            message = "faild";
        }
        std::cout << ret << std::endl;
    }
    else if(requestParser.getMethod() == "GET"){
        if(resource == "/"){
        resource = "html/index.html";
        }
        else{
            for(auto t:HttpRequestParser::DEFAULT_HTML){
                if(resource == t){
                    resource = "html/"+t+".html";
                    break;
                }
            }
        }
    }
    // default page
    
    
    // bad page
    

    // if(!requestResponser.resourceCheck(resource)){
    //     resource = "html/error.html";
    // }
    requestResponser.init(statuscode,message,resource);
    requestResponser.makeContent();
    send(fd, (char *)requestResponser.toString().c_str(),requestResponser.toString().size(), 0);
    // // std::cout << "Get data " << readBuf <<std::endl;
    // // 回显客户端发送的数据
    
    // send(fd, this->readBuf, size, 0);
}


int HttpConn::Read(int &errno_) {
    int totalSize = 0;
    int bytesRead = 0;
    memset(readBuf,'\0',READ_BUFFER);
    while (true) {
        ssize_t bytesRead = read(fd, readBuf + totalSize, READ_BUFFER - 1 - totalSize);

        if (bytesRead > 0) {
            totalSize += bytesRead;
        }
        else{
            if(bytesRead <0)
                errno_ = errno;
            // break;
            return bytesRead;
        }
    }
    // return bytesRead;
}


void HttpConn::Write()
{

}


