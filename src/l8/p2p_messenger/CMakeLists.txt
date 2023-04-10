cmake_minimum_required(VERSION 3.10)

project(p2p-messgener CXX)

set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SRC main.cpp p2p.h)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")
target_link_libraries("${PROJECT_NAME}" PRIVATE ${Boost_LIBRARIES} pthread)


