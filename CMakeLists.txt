cmake_minimum_required(VERSION 3.13)
project(ad-hoc_messenger)
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)

# also depends on crow (https://github.com/fckxorg/crow)
# and on libpqxx (https://github.com/fckxorg/libpqxx)
# libpqxx.a library file in /usr/local/lib should be renamed to libllibpqxx.a 
# for linker to work properly

find_package(Boost COMPONENTS system REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(main ${Boost_SYSTEM_LIBRARY} pthread ZLIB::ZLIB)
