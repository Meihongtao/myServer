#include<iostream>
#include<chrono>


// 定时器类
class Timer {
public:


    bool operator>(const Timer& other) const {
        return expiry > other.expiry;
    }
public:
    int fd;
    std::chrono::steady_clock::time_point expiry;  // 定时器过期时间
};