cmake_minimum_required(VERSION 3.10)

project(beast-ssl-client CXX)

set(${PROJECT_NAME}_SRC main.cpp root_certificates.hpp)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

find_package(OpenSSL)
add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")


if(WIN32)
  target_link_libraries("${PROJECT_NAME}" wsock32 ws2_32)
else()
  target_link_libraries("${PROJECT_NAME}" pthread)
  target_link_libraries("${PROJECT_NAME}" OpenSSL::SSL)
endif()
