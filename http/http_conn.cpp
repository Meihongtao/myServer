

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

int HttpConn::Close()
{
    this->requestResponser.unmapFile();
    if(isClose == false){
        isClose = true; 
        HttpConn::user_count -= 1;
        timer = nullptr;
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

    requestResponser.init(resource, requestParser.isKeepAlive(), 200,requestParser.getMethod(),message);
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
