#include "http_request.h"

const std::unordered_set<std::string> HttpRequestParser::DEFAULT_HTML{
    "/index",
    "/index2",
    "/video",
    "/picture",
    "/error",
    "/default",
};
const std::unordered_map<std::string, HttpRequestParser::action> HttpRequestParser::ACTION_MAPPING{
    {"/Login", HttpRequestParser::action::LOGIN}, {"/Register", HttpRequestParser::action::REGISTER}};


void HttpRequestParser::init(){
    method = path = body = queryString = "";
    headers.clear();
    lines.clear();
}

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
    return true;
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
    body += request_bodyline;
}

bool HttpRequestParser::Login(std::string username, std::string password_)
{
    bool flag = true;
    MYSQL *sql = (SqlPool::getInstance())->getConn();
    MYSQL_RES *res = nullptr;
    MYSQL_FIELD *fields = nullptr;
    unsigned int j = 0;
    char order[256] = {0};
    snprintf(order, 256, "SELECT username, password FROM user WHERE username='%s' LIMIT 1", username.c_str());

    assert(sql);
    // success = 0
    if (mysql_query(sql, order))
    {
        printf("查询执行失败\n");
        mysql_free_result(res);
        return false;
    }
    res = mysql_store_result(sql);
    j = mysql_num_fields(res);
    my_ulonglong rows = mysql_num_rows(res);
    if (rows == 0)
    {
        flag = false;
        printf("没有此用户\n");
    }
    fields = mysql_fetch_fields(res);
    while (MYSQL_ROW row = mysql_fetch_row(res))
    {
        printf("%s  %s\n", row[0], row[1]);
        std::string password(row[1]);

        if (password_ == password)
        {
            flag = true;
        }
        else
        {
            flag = false;
            printf("密码错误\n");
        }
    }

    (SqlPool::getInstance())->freeConn(sql);
    return flag;
}

bool HttpRequestParser::Register(std::string username, std::string password)
{
    return false;
}

bool HttpRequestParser::postHandler()
{
    bool ret = true;
    std::unordered_map<std::string, std::string> post_params;
    std::istringstream data_stream(this->getBody());
    std::string key_value;
    while (std::getline(data_stream, key_value, '&'))
    {
        size_t equal_pos = key_value.find('=');
        if (equal_pos != std::string::npos)
        {
            std::string key = key_value.substr(0, equal_pos);
            std::string value = key_value.substr(equal_pos + 1);
            post_params[key] = value;
        }
    }

    // 输出解析后的POST数据
    // for (const auto& pair : post_params) {
    //     std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
    //     std::cout << "-------------------------------" << std::endl;
    // }
    if (ACTION_MAPPING.count(getResource()))
    {
        int action_ = ACTION_MAPPING.find(getResource())->second;
        switch (action_)
        {
        case action::LOGIN:
            /* code */
            // std::cout << post_params.find("user")->second << "==" << post_params.find("password")->second << std::endl;
            ret = Login(post_params.find("user")->second, post_params.find("password")->second);
            break;
        case action::REGISTER:
            ret = Register(post_params.find("user")->second, post_params.find("password")->second);
            break;
        default:
            ret = false;
            break;
        }
    }
    else
    {
        return false;
    }
    return ret;
}

bool HttpRequestParser::isKeepAlive() const
{
    if (headers.count("Connection") == 1)
    {
        return headers.find("Connection")->second == "keep-alive";
    }
    return false;
}
