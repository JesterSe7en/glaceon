cmake_minimum_required(VERSION 3.27.8 FATAL_ERROR)

set(CMAKE_SYSTEM_VERSION 10.0 CACHE STRING "" FORCE)

include(ExternalProject)
project(Glaceon CXX)

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

# Global compiler options
if(MSVC)
  # remove default flags provided with CMake for MSVC
  set(CMAKE_CXX_FLAGS "")
  set(CMAKE_CXX_FLAGS_DEBUG "")
  set(CMAKE_CXX_FLAGS_RELEASE "")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "")
endif()

# Global linker options
if(MSVC)
  # remove default flags provided with CMake for MSVC
  set(CMAKE_EXE_LINKER_FLAGS "")
  set(CMAKE_MODULE_LINKER_FLAGS "")
  set(CMAKE_SHARED_LINKER_FLAGS "")
  set(CMAKE_STATIC_LINKER_FLAGS "")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS_DEBUG "${CMAKE_MODULE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_STATIC_LINKER_FLAGS_DEBUG "${CMAKE_STATIC_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_MODULE_LINKER_FLAGS}")
  set(CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_SHARED_LINKER_FLAGS}")
  set(CMAKE_STATIC_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_STATIC_LINKER_FLAGS}")
endif()

# Nuget packages function stub.
function(use_package TARGET PACKAGE VERSION)
  message(WARNING "No implementation of use_package. Create yours. "
                    "Package \"${PACKAGE}\" with version \"${VERSION}\" "
                    "for target \"${TARGET}\" is ignored!")
endfunction()

# Common utils
include(CMake/Utils.cmake)

# Additional Global Settings(add specific info there)
include(CMake/GlobalSettingsInclude.cmake OPTIONAL)

# Use solution folders feature
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


# Check that submodules are initialized
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
  # Update submodules as needed
  option(GIT_SUBMODULE "Check submodules during build" ON)
  if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
    if(NOT GIT_SUBMOD_RESULT EQUAL "0")
      message(FATAL_ERROR "git submodule update --init --recursive failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
  endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/glfw/CMakeLists.txt")
  message(FATAL_ERROR "The glfw submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/spdlog/CMakeLists.txt")
  message(FATAL_ERROR "The spdlog submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/extern/spdlog/CMakeLists.txt")
  message(FATAL_ERROR "The imgui submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

# include(ExternalProject) is needed for this
#ExternalProject_Add(spdlog
#        PREFIX spdlog
#        SOURCE_DIR ${PROJECT_SOURCE_DIR}/extern/spdlog
#        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
#        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
#        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
#        -DCMAKE_INSTALL_PREFIX=${STAGING_DIR}
#        -DSPDLOG_BUILD_SHARED=OFF
#)


# # ##############################################################################
# Sub-projects
# # ##############################################################################
add_subdirectory(Glaceon)
add_subdirectory(SandboxApp)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)

set(SPDLOG_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/spdlog)

add_subdirectory(extern/imgui)


## Example usage:
##  mkdir build
##  cd build
##  cmake -g "Visual Studio 14 2015" ..
#
#cmake_minimum_required(VERSION 2.8)
#project(imgui_example_glfw_vulkan C CXX)
#
#if(NOT CMAKE_BUILD_TYPE)
#  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
#endif()
#
#set(CMAKE_CXX_STANDARD 11)
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DVK_PROTOTYPES")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVK_PROTOTYPES")
#
## GLFW
#set(GLFW_DIR ../../../glfw) # Set this to point to an up-to-date GLFW repo
#option(GLFW_BUILD_EXAMPLES "Build the GLFW example programs" OFF)
#option(GLFW_BUILD_TESTS "Build the GLFW test programs" OFF)
#option(GLFW_BUILD_DOCS "Build the GLFW documentation" OFF)
#option(GLFW_INSTALL "Generate installation target" OFF)
#option(GLFW_DOCUMENT_INTERNALS "Include internals in documentation" OFF)
#add_subdirectory(${GLFW_DIR} binary_dir EXCLUDE_FROM_ALL)
#include_directories(${GLFW_DIR}/include)
#
## Dear ImGui
#set(IMGUI_DIR ../../)
#include_directories(${IMGUI_DIR} ${IMGUI_DIR}/backends ..)
#
## Libraries
#find_package(Vulkan REQUIRED)
##find_library(VULKAN_LIBRARY
##NAMES vulkan vulkan-1)
##set(LIBRARIES "glfw;${VULKAN_LIBRARY}")
#set(LIBRARIES "glfw;Vulkan::Vulkan")
#
## Use vulkan headers from glfw:
#include_directories(${GLFW_DIR}/deps)
#
#file(GLOB sources *.cpp)
#
#add_executable(example_glfw_vulkan ${sources} ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp ${IMGUI_DIR}/backends/imgui_impl_vulkan.cpp ${IMGUI_DIR}/imgui.cpp ${IMGUI_DIR}/imgui_draw.cpp ${IMGUI_DIR}/imgui_demo.cpp ${IMGUI_DIR}/imgui_tables.cpp ${IMGUI_DIR}/imgui_widgets.cpp)
#target_link_libraries(example_glfw_vulkan ${LIBRARIES})
#target_compile_definitions(example_glfw_vulkan PUBLIC -DImTextureID=ImU64)
