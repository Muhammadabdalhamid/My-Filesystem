cmake_minimum_required(VERSION 3.20)

project(MyFilesystem VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/FileSysytem.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)
