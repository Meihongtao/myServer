#include "http_response.h"

std::string HttpResponse::toString() const{
    std::string response;
    response += "HTTP/1.1 " + std::to_string(statusCode) + " " + getStatusMessage(statusCode) + "\r\n";

    for (const auto& header : headers) {
        response += header.first + ": " + header.second + "\r\n";
    }

    response += "\r\n";
    response += body;
    return response;
}

void HttpResponse::init(int statusCode, const std::string& body){
        this->statusCode = statusCode;
        this->body = body;
        headers["Server"] = "MyServer";
        headers["Content-Length"] = std::to_string(body.length());
}
