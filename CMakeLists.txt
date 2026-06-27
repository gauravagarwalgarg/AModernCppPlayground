cmake_minimum_required(VERSION 3.16)
project(AModernCppPlayground LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic -Werror=return-type)

# Release optimizations
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O2 -march=native)
endif()

# Auto-discover all .cpp source files under src/
file(GLOB_RECURSE ALL_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/**/*.cpp")

foreach(SOURCE_FILE ${ALL_SOURCES})
    # Derive executable name from relative path
    file(RELATIVE_PATH REL_PATH "${CMAKE_SOURCE_DIR}/src" ${SOURCE_FILE})
    string(REPLACE "/" "_" EXEC_NAME ${REL_PATH})
    string(REPLACE ".cpp" "" EXEC_NAME ${EXEC_NAME})

    add_executable(${EXEC_NAME} ${SOURCE_FILE})
    target_link_libraries(${EXEC_NAME} PRIVATE pthread)

    # Enable C++20 concepts/ranges where supported
    target_compile_features(${EXEC_NAME} PRIVATE cxx_std_20)
endforeach()
