#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <queue>
#include <chrono>
#include <thread>
#include <memory>
#include <fcntl.h>

#include "../pool/threadsPool.h"
#include "../http/http_conn.h"
#include "./epoll_controller.h"
#include "../log/log.h"
#include "../timer/time_heap.h"

int HttpConn::user_count = 0;

class myServer
{
public:
    myServer(int port, int threadnums, int timeout, int sqlPort, const char *sqlUser, const char *sqlPwd,
             const char *dbName, int connPoolNum) : timeout(timeout), port(port), threadnums(threadnums)
    {
        epoller = std::make_unique<EpollController>();
        pool = std::make_unique<ThreadPool>(threadnums);
        heapTimer = std::make_unique<HeapTimer>();

        initListen();
        SqlPool::getInstance()->Init("localhost", sqlPort, sqlUser, sqlPwd, dbName, connPoolNum);
        // AsyncLogger &logger = AsyncLogger::getInstance();

        // logger.startLogging("../log.txt");
    };
    ~myServer();
    bool initListen();
    void start();
    void extentTime(HttpConn &httpconn);
    void readHandler(HttpConn &httpconn);
    void Process(HttpConn &httpconn);
    void connHandler();
    void writeHandler(HttpConn &httpconn);
    void closeHandler(HttpConn &httpconn);

private:
    int port;
    int threadnums;
    int timeout;
    int serverSocket;

    std::unique_ptr<HeapTimer> heapTimer;
    std::unique_ptr<EpollController> epoller;
    std::unique_ptr<ThreadPool> pool;
    HttpConn *users = new HttpConn[10000];
};