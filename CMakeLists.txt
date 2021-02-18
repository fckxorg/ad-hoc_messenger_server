cmake_minimum_required(VERSION 3.13)
project(ad-hoc_messenger)
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)

find_package(Boost COMPONENTS system REQUIRED)
find_package(ZLIB REQUIRED)
target_link_libraries(main ${Boost_SYSTEM_LIBRARY} pthread ZLIB::ZLIB)
