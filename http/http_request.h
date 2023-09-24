#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
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
    bool parse(const std::string &httpRequest);
    HttpRequestParser() : state(HttpRequestState::METHOD){};

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