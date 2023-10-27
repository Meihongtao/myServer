#include "sql_pool.h"

SqlPool *SqlPool::getInstance()
{
    static SqlPool sqlPool;
    return &sqlPool;
}

MYSQL *SqlPool::getConn()
{
    MYSQL *sql = nullptr;
    if(sql_queue.empty()){
        return sql;
    }
    sem_wait(&semID);
    // 保证队列完整弹出
    {
        std::lock_guard<std::mutex> locker(mtx);
        sql = sql_queue.front();
        sql_queue.pop();
        free_count -= 1;
    }
    return sql;
}

void SqlPool::freeConn(MYSQL *sql)
{
    if(sql){
        std::lock_guard<std::mutex> locker(mtx);
        sql_queue.emplace(sql);
        sem_post(&semID);
    }
}

int SqlPool::getfreeCount()
{
    std::lock_guard<std::mutex> locker(mtx);
    return sql_queue.size();
}

void SqlPool::Init(const char *host, int port, const char *user, const char *pwd, const char *dbName, int conn_size)
{
    // 创建sql连接
    for(int i=0;i<conn_size;i++){
        MYSQL *sql = nullptr;
        sql = mysql_init(sql);
        if(!sql){
            assert(sql);
        }
        sql = mysql_real_connect(sql,host,user,pwd,dbName,port,nullptr,0);
        if(!sql){
            assert(sql);
        }
        sql_queue.emplace(sql);
    }
    max_count = conn_size;
    free_count = conn_size;
    // 初始化信号量
    sem_init(&semID,0,max_count);
}

void SqlPool::closePool()
{
    std::lock_guard<std::mutex> locker(mtx);
    while(!sql_queue.empty()){
        MYSQL * item = sql_queue.front();
        sql_queue.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

SqlPool::SqlPool()
{
    max_count = 0;
    free_count = 0;
}

SqlPool::~SqlPool()
{
    closePool();
}
