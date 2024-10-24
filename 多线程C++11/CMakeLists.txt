cmake_minimum_required(VERSION 3.30)

project(multiThread) 

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER clang++)

add_executable(Chrono ./chrono.cc)
add_executable(Thread ./thread.cc)
add_executable(Condition ./condition_variable.cc)
add_executable(Atomic ./atomic.cc)
add_executable(ThreadPool ./threadpool.cc)
