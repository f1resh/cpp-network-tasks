cmake_minimum_required(VERSION 3.10)

project(libcurl-client CXX)

set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SRC main.cpp)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")


target_link_libraries("${PROJECT_NAME}" PRIVATE cpr::cpr)

