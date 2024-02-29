set(PROJECT_NAME Glaceon)

################################################################################
# Source groups
################################################################################
set(Header_Files
    "framework.h"
    "Glaceon.h"
    "pch.h"
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    "dllmain.cpp"
    "Glaceon.cpp"
    "pch.cpp"
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Glaceon)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION_RELEASE        "TRUE"
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "TRUE"
)
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
    "GLACEON_EXPORTS;"
    "_WINDOWS;"
    "_USRDLL;"
    "UNICODE;"
    "_UNICODE"
)

################################################################################
# Compile and link options
################################################################################
if(MSVC)
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
        $<$<CONFIG:Release>:
            /OPT:REF;
            /OPT:ICF
        >
        $<$<CONFIG:RelWithDebInfo>:
            /OPT:REF;
            /OPT:ICF
        >
        /DEBUG;
        /SUBSYSTEM:WINDOWS
    )
endif()


################################################################################
# Dependencies
################################################################################

find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/extern/spdlog/include)
add_dependencies(${PROJECT_NAME} spdlog)

