add_library(handlers)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

target_include_directories(handlers 
                           PUBLIC 
                            ${INCROOT}/handlers 
                            ${INCROOT}/request_helpers
                            ${LIBMONGOCXX_LIBRARIES} 
                            ${LIBBSONCXX_LIBRARIES}
                            ${LIBMONGOCXX_INCLUDE_DIRS}
                            ${LIBBSONCXX_INCLUDE_DIRS})

target_sources(handlers 
               PUBLIC
                ${INCROOT}/handlers/handlers.hpp
               PRIVATE
                handlers.cpp
               )

target_link_libraries(handlers PUBLIC
                      ${LIBMONGOCXX_LIBRARIES}
                      ${LIBBSONCXX_LIBRARIES})

