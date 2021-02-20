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

set(INCROOT ${PROJECT_SOURCE_DIR}/include/)

target_include_directories(main 
                           PUBLIC
                           ${LIBMONGOCXX_LIBRARIES} 
                           ${LIBBSONCXX_LIBRARIES}
                           ${LIBMONGOCXX_INCLUDE_DIRS}
                           ${LIBBSONCXX_INCLUDE_DIRS}
                           ${INCROOT})



target_link_libraries(main PUBLIC  
                     ${Boost_SYSTEM_LIBRARY} 
                     ${LIBBSONCXX_LIBRARIES} 
                     ${LIBMONGOCXX_LIBRARIES} 
                     pthread 
                     ZLIB::ZLIB) 

