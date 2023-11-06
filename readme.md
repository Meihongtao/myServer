# 实现技术点
## 后端
- IO复用技术，Reactor主从模式，主线程处理新建立的连接，从线程处理IO读写事件
-  实现线程池作为从线程，减少线程创建和销毁的开销
-  实现数据库连接池，完成数据库的高性能读写
-  基于状态机完成http解析，实现静态资源的Get请求解析(文本，图片)。结合数据库连接池实现对POST请求解析，完成登录注册功能
-  基于STL容器实现最小时间堆，实现对超时连接的销毁

## 前端
- 基于[Bootstrap](https://v3.bootcss.com/)框架完成前端界面的功能界面
- [jQuery](https://jquery.com/) Ajax异步登录

# 目录结构
```
├── http
│   ├── http_conn.cpp
│   ├── http_conn.h
│   ├── http_request.cpp
│   ├── http_request.h
│   ├── http_response.cpp
│   └── http_response.h
├── log
│   └── log.cpp
├── main.cpp
├── myserver
│   ├── epoll_controller.h
│   ├── server.cpp
│   └── server.h
├── pool
│   ├── sql_pool.cpp
│   ├── sql_pool.h
│   └── threadsPool.h
├── readme.md
├── sample
│   └── httpparse.cpp
├── timer
│   ├── time_heap.cpp
│   ├── timeheap.cpp
│   └── time_heap.h
└── utils
```

