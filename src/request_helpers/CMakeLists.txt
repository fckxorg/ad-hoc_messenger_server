add_library(request_helpers)

set(INCROOT ${PROJECT_SOURCE_DIR}/include/request_helpers)

target_include_directories(request_helpers PUBLIC ${INCROOT})

target_sources(request_helpers PUBLIC ${INCROOT}/request_helpers.hpp PRIVATE request_helpers.cpp)
