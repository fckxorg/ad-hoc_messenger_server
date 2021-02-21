#====================================================
# dependencies:
# - crow (https://github.com/fckxorg/crow)
# - mongodb
# - mongocxx
# - bsoncxx
# - Boost
# - ZLIB
# - pthread
# - gtest
#====================================================

cmake_minimum_required(VERSION 3.13)
project(ad-hoc_messenger)
set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "build with tests" OFF)

find_package(Boost COMPONENTS system REQUIRED)
find_package(ZLIB REQUIRED)
find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/)

add_subdirectory(src/handlers)


set(CURRENT_TARGET main.cpp)

add_executable(${CURRENT_TARGET} src/main.cpp)

target_include_directories(${CURRENT_TARGET} 
                           PUBLIC
                            ${LIBMONGOCXX_LIBRARIES} 
                            ${LIBBSONCXX_LIBRARIES}
                            ${LIBMONGOCXX_INCLUDE_DIRS}
                            ${LIBBSONCXX_INCLUDE_DIRS}
                            ${INCROOT})



target_link_libraries(${CURRENT_TARGET} 
                      PUBLIC  
                       ${Boost_SYSTEM_LIBRARY} 
                       ${LIBBSONCXX_LIBRARIES} 
                       ${LIBMONGOCXX_LIBRARIES} 
                       pthread 
                       ZLIB::ZLIB
                       handlers) 


