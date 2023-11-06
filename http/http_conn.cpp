

#include "http_conn.h"

HttpConn::~HttpConn()
{
}

void HttpConn::init(int fd_, TimerNode *timer_)
{
    fd = fd_;
    totalSize = 0;
    // 设置定时器
    timer = timer_;
    // 连接用户数
    HttpConn::user_count += 1;
    // 初始化http解析
    requestParser = HttpRequestParser();
    requestResponser = HttpResponse();
    isClose = false;
}

void HttpConn::Close()
{
    if(fd>3 && isClose == false){
        HttpConn::user_count -= 1;
        close(fd);
    }
}

bool HttpConn::process()
{
    std::string message = "success";
    std::string resource = "/";
    int statuscode = 200;
    if (totalSize <= 0)
    {
        return false;
    }
    requestParser.init();
    requestParser.parse(readBuf);
    // std::cout << readBuf << std::endl;
    // std::cout << requestParser.getMethod() << " " << requestParser.getResource() << " " << requestParser.getBody() << std::endl;
    // std::string s = "<head><body>Here is index page</body></head>";
    // requestResponser.init(200,s);
    resource = requestParser.getResource();
    if (requestParser.getMethod() == "POST")
    {
        bool ret = requestParser.postHandler();
        if (ret)
        {
            message = "success";
        }
        else
        {
            message = "faild";
        }
        std::cout << ret << std::endl;
    }
    else if (requestParser.getMethod() == "GET")
    {
        if (resource == "/")
        {
            resource = "html/index.html";
        }
        else
        {
            for (auto t : HttpRequestParser::DEFAULT_HTML)
            {
                if (resource == t)
                {
                    resource = "html/" + t + ".html";
                    break;
                }
            }
        }
    }

    requestResponser.init(resource, requestParser.isKeepAlive(), 200);
    requestResponser.MakeResponse(writeBuf);

    /* 响应头 */
    iov[0].iov_base = (char *)writeBuf.c_str();
    iov[0].iov_len = writeBuf.size();
    iov_count = 1;

    /* 文件 */
    if (requestResponser.getFileLen() > 0 && requestResponser.getFile())
    {
        // printf("here\n");
        iov[1].iov_base = requestResponser.getFile();
        iov[1].iov_len = requestResponser.getFileLen();
        iov_count = 2;
    }
    // LOG_DEBUG("filesize:%d, %d  to %d", response_.FileLen() , iovCnt_, ToWriteBytes());
    // return true;

    // requestResponser.init(statuscode,message,resource);
    // requestResponser.makeContent();
    // send(fd, (char *)requestResponser.toString().c_str(),requestResponser.toString().size(), 0);
    // // std::cout << "Get data " << readBuf <<std::endl;
    // // 回显客户端发送的数据

    // send(fd, this->readBuf, size, 0);
    return true;
}

int HttpConn::Read(int &errno_)
{
    totalSize = 0;
    int bytesRead = 0;
    memset(readBuf, '\0', READ_BUFFER);
    while (true)
    {
        ssize_t bytesRead = read(fd, readBuf + totalSize, READ_BUFFER - 1 - totalSize);

        if (bytesRead > 0)
        {
            totalSize += bytesRead;
        }
        else
        {
            if (bytesRead < 0)
                errno_ = errno;
            // break;
            return bytesRead;
        }
    }
    // return bytesRead;
}

int HttpConn::Write(int &writeError)
{
    // 文件传输
    int i = 0;
    int size = -1;
    while (1)
    {
        size = writev(fd, iov, iov_count);
        if (size <= 0)
        {
            writeError = errno;
            // AsyncLogger::getInstance().log(LogLevel::ERROR,strerror(errno));
            break;
        }
        if (iov[0].iov_len + iov[1].iov_len == 0)
        {
            
            break;
        } /* 传输结束 */
        else if (static_cast<size_t>(size) > iov[0].iov_len)
        {
            iov[1].iov_base = (uint8_t *)iov[1].iov_base + (size - iov[0].iov_len);
            iov[1].iov_len -= (size - iov[0].iov_len);
            if (iov[0].iov_len)
            {
                // writeBuff_.RetrieveAll();
                iov[0].iov_len = 0;
            }
        }
        else
        {
            iov[0].iov_base = (uint8_t *)iov[0].iov_base + size;
            iov[0].iov_len -= size;
            // writeBuff_.Retrieve(len);
        }
        // printf("%d\n", i++);
    }
    return size;
}

bool HttpConn::isKeepAlive() const
{
    return requestParser.isKeepAlive();
}

int HttpConn::toWriteBytes()
{
    if (iov[0].iov_base && iov[1].iov_base){
        return iov[0].iov_len + iov[1].iov_len;
    }
    else{
        // AsyncLogger::getInstance().log(LogLevel::ERROR,"nullptr for iov");
        return 1;
    }
   
}
