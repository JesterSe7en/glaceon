set(PROJECT_NAME SandboxApp)

################################################################################
# Source groups
################################################################################
set(Source_Files
        "Game.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(Header_Files
        "Game.h"
)

source_group("Header Files" FILES ${Header_Files})

set(ALL_FILES
        ${Source_Files}
        ${Header_Files}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE SandboxApp)

set_target_properties(${PROJECT_NAME} PROPERTIES
        VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "TRUE"
)
################################################################################
# Include directories
################################################################################
target_include_directories(${PROJECT_NAME} PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/../Glaceon")

################################################################################
# Compile definitions
################################################################################
target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
        "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
        "NDEBUG"
        ">"
        "$<$<CONFIG:RelWithDebInfo>:"
        "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
            /Oi;
            /Gy
            >
            $<$<CONFIG:RelWithDebInfo>:
            /Oi;
            /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
            /DEBUG
            >
            $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
            >
            $<$<CONFIG:RelWithDebInfo>:
            /DEBUG;
            /OPT:REF;
            /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
            -O3;
            -march=native
            >
            $<$<CONFIG:Debug>:
            -Og;
            -g;
            -Wall;
            -Wextra;
            -pedantic
            >
            $<$<CONFIG:RelWithDebInfo>:
            -O2;
            -g;
            -march=native
            >
    )
    target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
            -Wl,--strip-all
            >
            $<$<CONFIG:RelWithDebInfo>:
            -Wl,--strip-all
            >
    )
endif ()

################################################################################
# Dependencies
################################################################################
# Link with other targets.
target_link_libraries(${PROJECT_NAME} PRIVATE
        Glaceon
)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
        "Glaceon"
)
target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

target_link_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}//${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/")

