cmake_minimum_required(VERSION 3.10)

project(libhttp-server C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(${PROJECT_NAME}_SRC main.cpp auth.h)

source_group(source FILES ${${PROJECT_NAME}_SRC}})
find_package(libhttpserver)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/logins.txt ${CMAKE_CURRENT_BINARY_DIR}/logins.txt COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/key.pem ${CMAKE_CURRENT_BINARY_DIR}/key.pem COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/server.pem ${CMAKE_CURRENT_BINARY_DIR}/server.pem COPYONLY)

#find_package(OpenSSL)
add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
target_link_libraries("${PROJECT_NAME}" PRIVATE httpserver)

#if(WIN32)
#  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
#else()
#  target_link_libraries("${PROJECT_NAME}" pthread)
#  target_link_libraries("${PROJECT_NAME}" OpenSSL::SSL)
#endif()



