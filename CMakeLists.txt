set(CMAKE_BUILD_TYPE Debug)
project(MYSERVER)

add_executable(SERVER http/http_request.h http/http_request.cpp 
        http/http_response.h http/http_response.cpp 
        http/http_conn.h http/http_conn.cpp  
        pool/threadsPool.h pool/sql_pool.h pool/sql_pool.cpp 
        timer/time_heap.h 
        myserver/server.h myserver/server.cpp)

target_link_libraries(SERVER mysqlclient)