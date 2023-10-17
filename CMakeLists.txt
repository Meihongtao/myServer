project(MYSERVER)

add_executable(SERVER http/http_request.h http/http_request.cpp http/http_response.h http/http_response.cpp http/http_conn.h http/http_conn.cpp  pool/threadsPool.h timer/time_heap.h myserver/server.h myserver/server.cpp)