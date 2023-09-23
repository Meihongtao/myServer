#include "http_request.h"

bool HttpRequestParser::parse(const std::string &httpRequest)
{
    size_t startPos = 0;
    size_t endPos;

    while ((endPos = httpRequest.find("\r\n", startPos)) != std::string::npos)
    {
        std::string line = httpRequest.substr(startPos, endPos - startPos);
        lines.push_back(line);
        startPos = endPos + 2; // Move startPos to the beginning of the next line
    }

    // Add the last line (if any)
    if (startPos < httpRequest.size())
    {
        std::string line = httpRequest.substr(startPos);
        lines.push_back(line);
    }

    // Print the separated lines

    for (int i = 0, size_ = lines.size(); i < size_; i++)
    {
        if (i == 0)
        {
            parse_request_line(lines[i]);
        }
        else if (i == size_ - 1)
        {
            parse_request_body(lines[i]);
        }
        else
        {
            parse_request_header(lines[i]);
        }
    }
}

void HttpRequestParser::parseHeaderLine(const std::string &line)
{
    size_t colonPos = line.find(':');
    if (colonPos != std::string::npos)
    {
        std::string name = line.substr(0, colonPos);
        std::string value = line.substr(colonPos + 2); // Skip the ': ' characters
        headers[name] = value;
    }
}

void HttpRequestParser::parse_request_line(const std::string &request_line)
{
    for (int index = 0; index < request_line.size(); index++)
    {
        char c = request_line[index];
        switch (state)
        {
        case HttpRequestState::METHOD:
            if (isalpha(c))
            {
                method += c;
            }
            else if (c == ' ')
            {
                state = HttpRequestState::PATH;
            }
            else
            {
                state = HttpRequestState::ERROR;
            }
            break;
        case HttpRequestState::PATH:
            if (c != ' ' && c != '?')
            {
                path += c;
            }
            else if (c == '?')
            {
                state = HttpRequestState::QUERY_STRING;
            }
            else
            {
                state = HttpRequestState::HTTP_VERSION;
            }
            break;
        case HttpRequestState::QUERY_STRING:
            if (c != ' ')
            {
                queryString += c;
            }
            else
            {
                state = HttpRequestState::HTTP_VERSION;
            }
            break;
        case HttpRequestState::HTTP_VERSION:
            if (c != '\r')
            {
                version += c;
            }
            else
            {
                state = HttpRequestState::HEADER;
                if (index + 1 < request_line.size())
                {
                    index += 1;
                }
            }
            break;
        case HttpRequestState::ERROR:
            std::cerr << "Error parsing HTTP request" << std::endl;
            return;
        }
    }
}

void HttpRequestParser::parse_request_header(const std::string &request_headline)
{
    parseHeaderLine(request_headline);
}

void HttpRequestParser::parse_request_body(const std::string &request_bodyline)
{
    body = request_bodyline;
}
