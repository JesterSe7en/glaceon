set(PROJECT_NAME SandboxApp)

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

set(ROOT_NAMESPACE SandboxApp)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      VS_GLOBAL_KEYWORD "Win32Proj"
                      INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
                      INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "TRUE"
)

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

# Compile and link options
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE
                         $<$<CONFIG:Release>:-O3;-march=native>
                         $<$<CONFIG:Debug>:-Og;-g;-Wall;-Wextra;-pedantic>
                         $<$<CONFIG:RelWithDebInfo>:-O2;-g;-march=native>
  )
  target_link_options(${PROJECT_NAME} PRIVATE
                      $<$<CONFIG:Release>:-Wl,--strip-all>
                      $<$<CONFIG:RelWithDebInfo>:-Wl,--strip-all>
  )
endif()

# Dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
                      Glaceon
)
