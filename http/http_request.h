#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include<iostream>
#include<string>
#include<cstring>
#include<unordered_map>
#include<vector>
#include<mysql/mysql.h>

#include<unordered_set>
#include<sstream>
#include "../pool/sql_pool.h"

// class myTriplet{
// public:
//     myTriplet(std::string );
//     std::string content;
//     int code;
//     std::string message;
// }

enum class HttpRequestState
{
    METHOD,
    PATH,
    QUERY_STRING,
    HTTP_VERSION,
    HEADER,
    BODY,
    DONE,
    ERROR
};

class HttpRequestParser
{

public:
    enum action {LOGIN=0,REGISTER};
    static const std::unordered_set<std::string> DEFAULT_HTML;
    static const std::unordered_map<std::string,action> ACTION_MAPPING;
    bool parse(const std::string &httpRequest);
    HttpRequestParser() : state(HttpRequestState::METHOD){};
    bool postHandler();
    bool getHandler();
    std::string getMethod()
    {
        return method;
    }

    std::string getResource()
    {
        return path;
    }

    std::string getBody(){
        return body;
    }

private:

    void parse_request_header(const std::string &request_headline);
    void parse_request_body(const std::string &request_bodyline);
    void parse_request_line(const std::string &request_line);
    void parseHeaderLine(const std::string &line);
    bool Login(std::string username,std::string password);
    bool Register(std::string username,std::string password);
    

private:
    
   
    HttpRequestState state;
    // 方法
    std::string method;
    // 资源路径
    std::string path;
    // get请求参数
    std::string queryString;
    std::string version;
    // 请求头
    std::unordered_map<std::string, std::string> headers;
    std::string headerLine;
    std::string body;
    // 分行记录 /r/n
    std::vector<std::string> lines;
};

#endif