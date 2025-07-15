add_library(api STATIC
    src/cache_manager.cpp
)

target_include_directories(api PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# API зависит от core и CPR
target_link_libraries(api PUBLIC 
    core
    cpr::cpr
)