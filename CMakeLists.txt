cmake_minimum_required(VERSION 3.25)
project(example)

set(CMAKE_CXX_STANDARD 14)
find_package(Threads)

include_directories(.)

add_executable(example
        main.cpp can_driver.cpp)
target_link_libraries(example ${CMAKE_THREAD_LIBS_INIT})