cmake_minimum_required(VERSION 3.10)

project(p2p-messgener CXX)

set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SRC main.cpp)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")


