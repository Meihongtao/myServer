project(MYSERVER)

add_executable(SERVER http/http_conn.cpp pool/threadsPool.h timer/time_heap.h myserver/server.cpp)