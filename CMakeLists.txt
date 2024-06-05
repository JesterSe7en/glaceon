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
if (NOT CMAKE_VS_PLATFORM_NAME)
  set(CMAKE_VS_PLATFORM_NAME "x64")
endif ()
message("${CMAKE_VS_PLATFORM_NAME} architecture in use")

if (NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64"))
  message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported!")
endif ()

# Global configuration types
set(CMAKE_CONFIGURATION_TYPES
        "Debug"
        "Release"
        "RelWithDebInfo"
        CACHE STRING "" FORCE
)

# # Global compiler options
# if (MSVC)
#     # remove default flags provided with CMake for MSVC
#     set(CMAKE_CXX_FLAGS "")
#     set(CMAKE_CXX_FLAGS_DEBUG "")
#     set(CMAKE_CXX_FLAGS_RELEASE "")
#     set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
# endif ()
#
# # Global linker options
# if (MSVC)
#     # remove default flags provided with CMake for MSVC
#     set(CMAKE_EXE_LINKER_FLAGS "")
#     set(CMAKE_MODULE_LINKER_FLAGS "")
#     set(CMAKE_SHARED_LINKER_FLAGS "")
#     set(CMAKE_STATIC_LINKER_FLAGS "")
#     set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
#     set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
#     set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
#     set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
#     set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
#     set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
#     set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
#     set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
#     set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS}")
#     set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS}")
#     set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS}")
#     set(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_STATIC_LINKER_FLAGS}")
#     add_compile_options(
#             "/D_SILENCE_ALL_MS_EXT_DEPRECATION_WARNINGS"
#     )
# endif ()
#
# Nuget packages function stub.
function(use_package TARGET PACKAGE VERSION)
  message(WARNING "No implementation of use_package. Create yours. "
            "Package \"${PACKAGE}\" with version \"${VERSION}\" "
            "for target \"${TARGET}\" is ignored!")
endfunction()

# Common utils
include(CMake/Utils.cmake)

# Additional Global Settings(add specific info there)
# include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

# Use solution folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Check that submodules are initialized

find_package(Wayland QUIET)

if (WAYLAND_FOUND)
  set(GLFW_BUILD_WAYLAND ON)
  message(STATUS "Using Wayland for GLFW")
else ()
  set(GLFW_BUILD_WAYLAND OFF)
  message(STATUS "Using X11 or another backend for GLFW")
endif ()


# Sub-projects
add_subdirectory(Glaceon)
add_subdirectory(SandboxApp)


# Custom target to build all
#set(all_targets Glaceon SandboxApp)
#add_custom_target(All DEPENDS ${all_targets})
