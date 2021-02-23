add_library(libdbutils)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

target_include_directories(libdbutils 
                           PUBLIC 
                            ${LIBMONGOCXX_LIBRARIES} 
                            ${LIBBSONCXX_LIBRARIES}
                            ${LIBMONGOCXX_INCLUDE_DIRS}
                            ${LIBBSONCXX_INCLUDE_DIRS}
                            ${INCROOT}/db_utils)

target_sources(libdbutils 
               PUBLIC 
                ${INCROOT}/db_utils/db_utils.hpp
               PRIVATE
                db_utils.cpp)

target_link_libraries(libhandlers PUBLIC
                      ${LIBMONGOCXX_LIBRARIES}
                      ${LIBBSONCXX_LIBRARIES})
