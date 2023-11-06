#include "http_response.h"

const std::unordered_map<std::string, std::string> HttpResponse::SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css "},
    {".js", "text/javascript "},
};

const std::unordered_map<int, std::string> HttpResponse::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};

const std::unordered_map<int, std::string> HttpResponse::CODE_PATH = {
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};

std::string HttpResponse::toString() const
{
    // std::string response;
    // response += "HTTP/1.1 " + std::to_string(statusCode) + " " + getStatusMessage(statusCode) + "\r\n";

    // for (const auto &header : headers)
    // {
    //     response += header.first + ": " + header.second + "\r\n";
    // }

    // response += "\r\n";
    // response += body;
    // return response.c_str();
    return "";
}

HttpResponse::~HttpResponse()
{
    if (mmFile)
    {
        munmap(mmFile, mmFileStat.st_size);
        mmFile = nullptr;
    }
}

void HttpResponse::init(std::string resource_, bool isKeepAlive, int code)
{
    if (resource_ == "")
    {
        AsyncLogger::getInstance().log(LogLevel::WARNING, "request null resource");
    }
    if (mmFile)
    {
        munmap(mmFile, mmFileStat.st_size);
        mmFile = nullptr;
    }
    statusCode = code;
    isKeepAlive = isKeepAlive;
    resource = resource_;

    mmFile = nullptr;
    mmFileStat = {0};
}

// bool HttpResponse::resourceCheck(const std::string path)
// {
//     std::cout << (PATH + path).c_str() << std::endl;
//     if (std::file::exists((PATH + path).c_str()))
//     {
//         return true;
//     }
//     return false;
// }

// bool HttpResponse::addHeader(std::string &buf)
// {
//     buf += "HTTP/1.1 " + std::to_string(statusCode) + " " + getStatusMessage(statusCode) + "\r\n";

//     for (const auto &header : headers)
//     {
//         buf += header.first + ": " + header.second + "\r\n";
//     }

//     buf += "\r\n";
//     // buf += body;
//     return true;
// }

void HttpResponse::addStateLine(std::string &buf)
{
    std::string status;
    if (CODE_STATUS.count(statusCode) == 1)
    {
        status = CODE_STATUS.find(statusCode)->second;
    }
    else
    {
        statusCode = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buf += "HTTP/1.1 " + std::to_string(statusCode) + " " + status + "\r\n";
}

void HttpResponse::addHeader(std::string &buf)
{
    buf += "Connection: ";
    if (isKeepAlive)
    {
        buf += "keep-alive\r\n";
        buf += "keep-alive: max=6, timeout=120\r\n";
    }
    else
    {
        buf += "close\r\n";
    }
    // buf += "Content-type: " + "txt" + "\r\n";
}

void HttpResponse::MakeResponse(std::string &buff)
{
    /* 判断请求的资源文件 */
    buff.clear();
    if (stat((PATH + resource).c_str(), &mmFileStat) < 0 || S_ISDIR(mmFileStat.st_mode))
    {
        statusCode = 404;
    }
    else if (!(mmFileStat.st_mode & S_IROTH))
    {
        statusCode = 403;
    }
    else if (statusCode == -1)
    {
        statusCode = 200;
    }
    // ErrorHtml_();
    addStateLine(buff);
    addHeader(buff);
    addContent(buff);
}

void HttpResponse::addContent(std::string &buff)
{

    // this->statusCode = code;
    // printf("%s\n", (PATH + resource).c_str());
    // 打开文件以供读取
    // if (std::filesystem::exists((PATH + resource).c_str()))
    int srcFd = open((PATH + resource).c_str(), O_RDONLY);
    if (srcFd < 0)
    {
        printf("srcFd File NotFound!\n");
        AsyncLogger::getInstance().log(LogLevel::ERROR, PATH + resource + " Not Found");
        return;
    }

    /* 将文件映射到内存提高文件的访问速度
        MAP_PRIVATE 建立一个写入时拷贝的私有映射*/

    auto mmRet = (int *)mmap(0, mmFileStat.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if (mmRet == MAP_FAILED)
    {
        mmFile = nullptr;
        // printf(" %s mmRet File NotFound! buffsize-%ld\n",(PATH + resource).c_str(),mmFileStat.st_size);
        // AsyncLogger::getInstance().log(LogLevel::ERROR,PATH + resource + " Not Found");
        // std::cout << (char *)mmRet << std::endl;
        return;
    }
    mmFile = (char *)mmRet;
    close(srcFd);
    buff += "Content-length: " + std::to_string(mmFileStat.st_size) + "\r\n\r\n";
}

char *HttpResponse::getFile()
{
    return mmFile;
}

size_t HttpResponse::getFileLen() const
{
    return mmFileStat.st_size;
}
