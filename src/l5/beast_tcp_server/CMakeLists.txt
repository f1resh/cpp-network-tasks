cmake_minimum_required(VERSION 3.10)

project(beast-tcp-server C CXX)

set(${PROJECT_NAME}_SRC main.cpp)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")

if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
else()
  target_link_libraries("${PROJECT_NAME}" pthread)
endif()

