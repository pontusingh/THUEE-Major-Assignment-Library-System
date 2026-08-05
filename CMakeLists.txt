cmake_minimum_required(VERSION 3.15)
project(LibraryManagementSystem CXX)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 告诉编译器头文件 (.h) 都在 include 目录及其子目录下
include_directories(include)
include_directories(include/interfaces)
include_directories(include/models)
include_directories(include/core)
include_directories(include/stats)
include_directories(include/storage)
include_directories(include/exceptions)
include_directories(include/ui)

# 自动收集 src 目录及其所有子目录下的 .cpp 文件
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# 将 main.cpp 和收集到的 src 文件编译成可执行程序
add_executable(LibrarySystem main.cpp ${SRC_FILES})

# 构建后把 data/ 目录复制到可执行文件旁，保证运行时相对路径 data/*.txt 自洽
add_custom_command(TARGET LibrarySystem POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:LibrarySystem>/data)