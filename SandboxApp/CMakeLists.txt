cmake_minimum_required(VERSION 3.21)
project(SandboxApp)

# Source groups
set(Source_Files
        Game.cpp
)
source_group("Source Files" FILES ${Source_Files})

set(Header_Files
        Game.h
)
source_group("Header Files" FILES ${Header_Files})

set(ALL_FILES
        ${Source_Files}
        ${Header_Files}
)

# Target
add_executable(${PROJECT_NAME} ${ALL_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Glaceon"
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:_DEBUG>"
        "$<$<CONFIG:Release>:NDEBUG>"
        "$<$<CONFIG:RelWithDebInfo>:NDEBUG>"
        "_CONSOLE;UNICODE;_UNICODE"
)

# Compile and link options for Clang/GNU
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:-O3;-march=native>
            $<$<CONFIG:Debug>:-Og;-g;-Wall;-Wextra;-pedantic>
            $<$<CONFIG:RelWithDebInfo>:-O2;-g;-march=native>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:-Wl,--strip-all>
            $<$<CONFIG:RelWithDebInfo>:-Wl,--strip-all>
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/O2 /Zc:preprocessor>
            $<$<CONFIG:Debug>:/Zi /W4 /D_CRT_SECURE_NO_WARNINGS /Zc:preprocessor>
            $<$<CONFIG:RelWithDebInfo>:/O2 /Zi /Zc:preprocessor>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/DEBUG:NONE /OPT:REF>
            $<$<CONFIG:RelWithDebInfo>:/DEBUG:NONE /OPT:REF>
    )
endif()

# Dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
        Glaceon
)

message(STATUS "SandboxApp: GLACEON_BUILD_DIR: ${GLACEON_BUILD_DIR}")

# Need to copy all dll, libs, and pdb files to the build directory
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${GLACEON_BUILD_DIR}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Glaceon DLLs, LIBs, and PDBs to build directory..."
)

# Post-build steps (copies over the glaceon dll to the build directory)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:Glaceon>
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying Glaceon DLL to build directory..."
)
