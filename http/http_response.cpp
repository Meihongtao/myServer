#include "http_response.h"

std::string HttpResponse::toString() const
{
    std::string response;
    response += "HTTP/1.1 " + std::to_string(statusCode) + " " + getStatusMessage(statusCode) + "\r\n";

    for (const auto &header : headers)
    {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";
    response += body;
    return response;
}

void HttpResponse::init(int statusCode, const std::string &body, const std::string resource)
{
    this->statusCode = statusCode;
    this->body = body;
    this->resource = resource;
}

bool HttpResponse::resourceCheck(const std::string path)
{
    std::cout << (PATH + path).c_str() << std::endl;
    if (std::filesystem::exists((PATH + path).c_str()))
    {
        return true;
    }
    return false;
}

bool HttpResponse::makeContent()
{

    // this->statusCode = code;
    // printf("%s\n",(PATH+path).c_str());
    // 打开文件以供读取
    if (std::filesystem::exists((PATH + resource).c_str()))
    {
        int fd = open((PATH + resource).c_str(), O_RDONLY);
        if (fd == -1)
        {
            std::cerr << (PATH + resource).c_str() << "\t"
                      << "无法打开文件" << std::endl;
            return false;
        }

        // 获取文件的大小
        struct stat file_stat;

        if (fstat(fd, &file_stat) == -1)
        {
            std::cerr << "无法获取文件信息" << std::endl;
            close(fd);
            return false;
        }

        // 使用 mmap 将文件映射到内存
        char *mapped_data = (char *)mmap(nullptr, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        if (mapped_data == MAP_FAILED)
        {
            std::cerr << "mmap 失败" << std::endl;
            close(fd);
            return false;
        }

        // 使用映射的数据进行操作
        this->body = mapped_data;
        // 解除映射并关闭文件
        munmap(mapped_data, file_stat.st_size);
        close(fd);
        headers["Server"] = "MyServer";
        headers["Content-Length"] = std::to_string(body.length());

        return true;
    }
    else{
        headers["Server"] = "MyServer";
        headers["Content-Length"] = std::to_string(body.length());
        return true;
    }

    return true;
}
