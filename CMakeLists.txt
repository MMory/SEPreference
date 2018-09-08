cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-O2 -Wall")

include_directories(external/include)

add_library(sepreference SHARED telegramDescriber.cpp simulatorExchangeSender.cpp)

target_link_libraries(sepreference)
