add_library(libmodels)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

target_include_directories(libmodels 
                           PUBLIC 
                            ${INCROOT}/mongo_odm
                            ${INCROOT}/models
                            ${LIBMONGOCXX_LIBRARIES} 
                            ${LIBBSONCXX_LIBRARIES}
                            ${LIBMONGOCXX_INCLUDE_DIRS}
                            ${LIBBSONCXX_INCLUDE_DIRS})

target_sources(libmodels 
               PUBLIC
                ${INCROOT}/models/models.hpp
               PRIVATE
                models.cpp
               )
target_link_libraries(libmodels PUBLIC
                      ${LIBMONGOCXX_LIBRARIES}
                      ${LIBBSONCXX_LIBRARIES})
