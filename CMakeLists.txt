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
set(TEST_TARGETS request_helpers mongo_odm handlers) # add handlers later

add_subdirectory(src/handlers)
add_subdirectory(src/models)
add_subdirectory(src/request_helpers)


if(BUILD_TESTS)
    set(CURRENT_TARGETS ${TEST_TARGETS})
    add_subdirectory(lib/googletest)
else()
    set(CURRENT_TARGETS main)
    add_executable(${CURRENT_TARGETS} src/main.cpp)
endif()

foreach(CURRENT_TARGET ${CURRENT_TARGETS})
    if(BUILD_TESTS)
        add_executable(${CURRENT_TARGET} test/${CURRENT_TARGET}_unittest.cpp)
        target_include_directories(${CURRENT_TARGET} PUBLIC lib/googletest/googletest/include)
        target_link_libraries(${CURRENT_TARGET} PUBLIC gtest)

        add_custom_command(TARGET ${CURRENT_TARGET} POST_BUILD COMMAND ${PROJECT_BINARY_DIR}/${CURRENT_TARGET})
    endif()

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
                           libhandlers
                           librequesthelpers
                           libmodels)

    
endforeach(CURRENT_TARGET)
