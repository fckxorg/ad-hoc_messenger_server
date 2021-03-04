add_library(librequesthelpers)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

target_include_directories(librequesthelpers 
                           PUBLIC 
                            ${INCROOT}/request_helpers)

target_sources(librequesthelpers
               PUBLIC
                ${INCROOT}/request_helpers/request_helpers.hpp
               PRIVATE
                request_helpers.cpp)
