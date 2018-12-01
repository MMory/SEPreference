cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)
find_package(Threads)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

include_directories(external/include)

add_library(sepreference SHARED telegramDescriber.cpp simulatorExchangeSender.cpp telegram.cpp sepsend.cpp)

target_link_libraries(sepreference ${CMAKE_THREAD_LIBS_INIT})

add_executable(jsonexample jsonexample.cpp)

target_link_libraries(jsonexample sepreference)
