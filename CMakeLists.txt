set(CMAKE_BUILD_TYPE Debug)
# Enable C++11
set(CMAKE_CXX_STANDARD 14)

# if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lc++ -lc++abi")
# endif()

# add_custom_target(cpplint echo '${PINE_LINT_FILES}' | xargs -n12 -P8
#         ${CPPLINT_BIN}
#         --verbose=2 --quiet
#         --linelength=120
#         --filter=-legal/copyright,-build/include_subdir,-readability/casting
#         )

project(MYSERVER)


add_executable(SERVER http/http_request.h http/http_request.cpp 
        http/http_response.h http/http_response.cpp 
        http/http_conn.h http/http_conn.cpp  
        pool/threadsPool.h pool/sql_pool.h pool/sql_pool.cpp 
        timer/time_heap.h 
        timer/time_heap.cpp
        log/log.h
        myserver/server.h myserver/server.cpp)

target_link_libraries(SERVER mysqlclient)