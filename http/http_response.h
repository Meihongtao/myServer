#include<cstring>
#include<vector>
#include<map>
#include<string>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<iostream>
#include<unistd.h>
#include<filesystem>
class HttpResponse{

public:
    HttpResponse(){};
    void init(int statusCode, const std::string& body,const std::string resource);
    bool resourceCheck(std::string path);
    bool makeContent();
    std::string toString() const;
    

private:
    std::string PATH = "../static_resources/";
    int statusCode;
    std::string resource;
    std::string body,message;
    std::map<std::string, std::string> headers;

    static std::string getStatusMessage(int statusCode) {
        switch (statusCode) {
            case 200:
                return "OK";
            case 404:
                return "Not Found";
            // Add more status codes as needed
            default:
                return "Unknown";
        }
    }
};

