cmake_minimum_required(VERSION 3.10)

project(pcpp-sniffer C CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

include_directories("/usr/local/include/pcapplusplus")

if (NOT WIN32)
    find_package(PcapPlusPlus REQUIRED)
else()
    # Locally-installed.
endif()

set(${PROJECT_NAME}_SRC main.cpp)

source_group(source FILES ${${PROJECT_NAME}_SRC}})

add_executable("${PROJECT_NAME}" "${${PROJECT_NAME}_SRC}")

target_include_directories("${PROJECT_NAME}" SYSTEM PRIVATE)

if (NOT WIN32)
    target_link_libraries(${PROJECT_NAME} ${PcapPlusPlus_LIBRARIES})
else()
    #windows
endif()
