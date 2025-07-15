add_library(core STATIC
    src/route_info.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Если core использует json
target_link_libraries(core PUBLIC nlohmann_json::nlohmann_json)