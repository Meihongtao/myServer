#include<cstring>
#include<vector>
#include<map>
#include<assert.h>
#include<string>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include<unistd.h>
// #include<experimental/filesystem>
#include<filesystem>
#include<unordered_map>

#include "../log/log.h"

class HttpResponse{

public:
    HttpResponse(){};
    ~HttpResponse();
    void init(std::string resouces, bool isKeepAlive, int code);
    bool resourceCheck(std::string path);
    // bool addHeader(std::string &buf);
    void addStateLine(std::string & buf);
    void addHeader(std::string &buf);
    void addContent(std::string &buf);
    bool isKeepAlive;
    void MakeResponse(std::string &buf);
    char* getFile();
    size_t getFileLen() const;
    std::string toString() const;
    

private:
    std::string PATH = "../static_resources/";
    int statusCode;
    std::string resource;
    // std::string body,message;
    // 文件指针
    char* mmFile; 
    // 文件描述符
    struct stat mmFileStat;
    // std::map<std::string, std::string> headers;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;

};

