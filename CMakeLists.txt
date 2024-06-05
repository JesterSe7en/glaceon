cmake_minimum_required(VERSION 3.22.1 FATAL_ERROR)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/extern/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
message(STATUS "Vcpkg toolchain file: ${CMAKE_TOOLCHAIN_FILE}")

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

include(ExternalProject)
project(Glaceon CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Set target arch type if empty. Visual studio solution generator provides it.
if(NOT CMAKE_VS_PLATFORM_NAME)
    set(CMAKE_VS_PLATFORM_NAME "x64")
endif()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
    message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif()

# Global configuration types
set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    "RelWithDebInfo"
    CACHE STRING "" FORCE
)

# Common utils
include(CMake/Utils.cmake)

# Use solution folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

find_package(Wayland QUIET)

if(WAYLAND_FOUND)
    set(GLFW_BUILD_WAYLAND ON)
    message(STATUS "Using Wayland for GLFW")
else()
    set(GLFW_BUILD_WAYLAND OFF)
    message(STATUS "Using X11 or another backend for GLFW")
endif()


# Sub-projects
add_subdirectory(Glaceon)
add_subdirectory(SandboxApp)

# Custom target to build all
#set(all_targets Glaceon SandboxApp)
#add_custom_target(All DEPENDS ${all_targets})
