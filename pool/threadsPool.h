#ifndef THREADSPOOL_H
#define THREADSPOOL_H


#include<iostream>
#include<functional>
#include<mutex>
#include<queue>
#include<vector>
#include<thread>
#include<condition_variable>

class ThreadPool{
private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_queue;
    std::mutex m_mtx;
    int m_thread_num;
    std::condition_variable m_condition;
    // static ThreadPool *pool;
    bool stop;

    
   

public:
    ThreadPool(int thread_num):stop(false),m_thread_num(thread_num){
        int i =0;
        for(i=0;i<thread_num;i++){
            m_threads.emplace_back([this,i]{
                printf("Thread %d created !\n",i+1);
                
                while(true){
                    std::function<void()> func;
                    {
                    std::unique_lock<std::mutex> lock(m_mtx);
                    m_condition.wait(lock,[this]{
                        // stop 为真 或者 任务队列不为空
                        return stop || !m_queue.empty();
                    });
                    // 当stop为真 以及队列为空时 退出
                    if(stop && m_queue.empty()){
                        return;
                    }
                    func = std::move(m_queue.front());
                    // 出队
                    m_queue.pop();

                    }                 
                    // 执行
                    func();
                }
            });
        }
    }

    ThreadPool(const ThreadPool& ) = delete;

    // static ThreadPool *getInstance(int num){
    //     if(pool == NULL){
    //         pool = new ThreadPool(num);
    //     }
    //     return pool;
    // }

    template<typename Func, typename... Args>
    void addTask(Func&& F,Args&& ...args){
        {
            std::unique_lock<std::mutex> lock(m_mtx);
            m_queue.emplace([=]{
                F(args...);
            });
        }
        m_condition.notify_one();
    }

    ~ThreadPool(){
         {
            std::unique_lock<std::mutex> lock(m_mtx);
            stop = true;
        }
        m_condition.notify_all();
        for (std::thread& thread : m_threads) {
            thread.join();
        }
    }
    
};


#endif