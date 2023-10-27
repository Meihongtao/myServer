#ifndef SQLPOOL_H
#define SQLPOOL_H

#include<mysql/mysql.h>
#include<queue>
#include<mutex>
#include<semaphore.h>
#include<assert.h>

class SqlPool{
public:
    static SqlPool *getInstance();
    MYSQL * getConn();
    void freeConn(MYSQL *sql);
    int getfreeCount();
    void Init(const char* host,int port,
                const char* user,const char* pwd,
                const char* dbName, int conn_size);
    void closePool();
private:
    SqlPool();
    ~SqlPool();
    int max_count;
    int free_count;
    std::queue<MYSQL *> sql_queue;
    std::mutex mtx;
    sem_t semID;
};



#endif
