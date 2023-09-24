#include<cstring>
#include<vector>
#include<map>
#include<string>

class HttpResponse{

public:
    HttpResponse(){};
    void init(int statusCode, const std::string& body);

    std::string toString() const;

private:
    int statusCode;
    std::string body;
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

