cmake_minimum_required(VERSION 3.13)
project(ad-hoc_messenger)
set(CMAKE_CXX_STANDARD 20)

add_executable(main main.cpp)

# also depends on crow (https://github.com/fckxorg/crow)
# and on mongodb

find_package(Boost COMPONENTS system REQUIRED)
find_package(ZLIB REQUIRED)
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

include_directories(${LIBMONGOCXX_LIBRARIES})
include_directories(${LIBBSONCXX_LIBRARIES})
include_directories(${LIBMONGOCXX_INCLUDE_DIRS})
include_directories(${LIBBSONCXX_INCLUDE_DIRS})

target_link_libraries(main 
                     ${Boost_SYSTEM_LIBRARY} 
                     ${LIBBSONCXX_LIBRARIES} 
                     ${LIBMONGOCXX_LIBRARIES} 
                     pthread 
                     ZLIB::ZLIB) 
