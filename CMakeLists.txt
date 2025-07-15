cmake_minimum_required(VERSION 3.20)
project(cpr_example)

set(CMAKE_CXX_STANDARD 17)

# Настройка типа сборки
set(DEFAULT_BUILD_TYPE "Release")
if(EXISTS "${CMAKE_SOURCE_DIR}/.git")
  set(DEFAULT_BUILD_TYPE "Debug")
endif()
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Подключаем подпроекты
add_subdirectory(libs/core)
add_subdirectory(libs/api)
add_subdirectory(app)

# Настройка выходных директорий
if(WIN32)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

# Зависимости
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG 1.11.0)
FetchContent_Declare(nlohmann_json GIT_REPOSITORY https://github.com/nlohmann/json
                         GIT_TAG v3.11.3)
FetchContent_MakeAvailable(cpr nlohmann_json)