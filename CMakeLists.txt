cmake_minimum_required(VERSION 4.0.3)  # Reasonable modern baseline
project(swarm-contracts-simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable all warnings
if (MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(-fcolor-diagnostics)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        add_compile_options(-fdiagnostics-color=always)
    endif()
endif()

# Include headers from "include" directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Collect source files (you can list them manually if you prefer)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
)

# Build the executable
add_executable(${PROJECT_NAME} ${SOURCES})
