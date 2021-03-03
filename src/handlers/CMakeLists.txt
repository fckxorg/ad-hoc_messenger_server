add_library(libhandlers)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

find_package(libmongocxx REQUIRED)
find_package(libbsoncxx REQUIRED)

target_include_directories(libhandlers 
                           PUBLIC 
                            ${INCROOT}/handlers 
                            ${INCROOT}/request_helpers
                            ${INCROOT}/db_utils
                            ${INCROOT}/mongo_odm
                            ${INCROOT}/models
                            ${LIBMONGOCXX_LIBRARIES} 
                            ${LIBBSONCXX_LIBRARIES}
                            ${LIBMONGOCXX_INCLUDE_DIRS}
                            ${LIBBSONCXX_INCLUDE_DIRS})

target_sources(libhandlers 
               PUBLIC
                ${INCROOT}/handlers/handlers.hpp
               PRIVATE
                handlers.cpp
               )

target_link_libraries(libhandlers PUBLIC
                      ${LIBMONGOCXX_LIBRARIES}
                      ${LIBBSONCXX_LIBRARIES}
                      libdbutils
                      libmodels)

