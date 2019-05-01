cmake_minimum_required(VERSION 3.9)
set(CMAKE_CXX_STANDARD 11)
find_package(Threads)
find_package(Boost 1.63.0 REQUIRED COMPONENTS system)

include_directories(external/include)

add_library(sepreference SHARED telegramDescriber.cpp simulatorExchangeSender.cpp telegram.cpp)

target_link_libraries(sepreference ${CMAKE_THREAD_LIBS_INIT} ${Boost_LIBRARIES})

add_executable(jsonexample jsonexample.cpp)

target_link_libraries(jsonexample sepreference)
