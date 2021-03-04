add_library(libhelpers)

set(INCROOT ${PROJECT_SOURCE_DIR}/include)

target_sources(libdbutils 
               PUBLIC 
                ${INCROOT}/db_utils/db_utils.hpp
               PRIVATE
                db_utils.cpp)

