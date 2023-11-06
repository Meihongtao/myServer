#ifndef MY_LOG_H
#define MY_LOG_H

#include <iostream>
#include <fstream>
#include <string.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// 日志等级枚举
enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

// 日志条目结构
struct LogEntry {
    LogLevel level;
    std::string message;
};

// 单例异步日志类
class AsyncLogger {
public:
    static AsyncLogger& getInstance() {
        static AsyncLogger instance;
        return instance;
    }

    // 启动日志线程
    void startLogging(const std::string& logFileName) {
        logFile.open(logFileName, std::ios::app); // 追加方式打开日志文件
        loggingThread = std::thread([this] { logThreadFunction(); });
    }

    // 停止日志线程
    void stopLogging() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopFlag = true;
        }
        condition.notify_one();
        loggingThread.join();
        logFile.close(); // 关闭日志文件
    }

    // 添加日志条目到队列
    void log(LogLevel level, const std::string& message) {
        std::lock_guard<std::mutex> lock(queueMutex);
        logQueue.push({ level, message });
        condition.notify_one();
    }

private:
    AsyncLogger() : stopFlag(false) {}

    // 日志线程函数
    void logThreadFunction() {
        while (true) {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] { return !logQueue.empty() || stopFlag; });

            if (stopFlag) {
                break;
            }

            LogEntry entry = logQueue.front();
            logQueue.pop();
            // 实际的日志输出到文件
            logFile << "[" << getLogLevelString(entry.level) << "] " << entry.message << std::endl;
            lock.unlock();
        }
    }

    // 将日志等级转换为字符串
    std::string getLogLevelString(LogLevel level) {
        switch (level) {
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            default:
                return "UNKNOWN";
        }
    }

    std::thread loggingThread;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::queue<LogEntry> logQueue;
    bool stopFlag;
    std::ofstream logFile; // 输出日志的文件流
};

// int main() {
//     AsyncLogger& logger = AsyncLogger::getInstance();
//     logger.startLogging("log.txt"); // 指定日志文件名

//     // 模拟日志输出
//     for (int i = 0; i < 10; ++i) {
//         logger.log(LogLevel::INFO, "Log entry " + std::to_string(i));
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     }

//     // 停止日志线程
//     logger.stopLogging();

//     return 0;
// }

#endif
