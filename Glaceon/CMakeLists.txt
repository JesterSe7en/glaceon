set(PROJECT_NAME Glaceon)

# Source groups
set(Header_Files
        framework.h
        Glaceon.h
        pch.h
        Base.h
        Logger.h
        Application.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imconfig.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_internal.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imstb_rectpack.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imstb_textedit.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/imstb_truetype.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.h
        ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_vulkan.h
        VulkanRenderer/VulkanDevice.h
        VulkanRenderer/VulkanContext.h
        VulkanRenderer/VulkanBackend.h
        VulkanRenderer/VulkanSwapChain.h
        VulkanRenderer/VulkanRenderPass.h
        VulkanRenderer/VulkanUtils.h
        VulkanRenderer/VulkanPipeline.h
        VulkanRenderer/VulkanCommandPool.h
        VulkanRenderer/VulkanSync.h
        Scene.h
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
        dllmain.cpp
        Glaceon.cpp
        pch.cpp
        Logger.cpp
        Application.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_demo.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/extern/imgui/backends/imgui_impl_vulkan.cpp
        VulkanRenderer/VulkanBackend.cpp
        VulkanRenderer/VulkanContext.cpp
        VulkanRenderer/VulkanDevice.cpp
        VulkanRenderer/VulkanSwapChain.cpp
        VulkanRenderer/VulkanRenderPass.cpp
        VulkanRenderer/VulkanUtils.cpp
        VulkanRenderer/VulkanPipeline.cpp
        VulkanRenderer/VulkanCommandPool.cpp
        VulkanRenderer/VulkanSync.cpp
        Scene.cpp
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
        ${Header_Files}
        ${Source_Files}
        TriangleMesh.cpp
        TriangleMesh.h
        SquareMesh.cpp
        SquareMesh.h
        StarMesh.cpp
        StarMesh.h
        VertexBufferCollection.cpp
        VertexBufferCollection.h
        VulkanRenderer/VulkanDescriptorPool.cpp
        VulkanRenderer/VulkanDescriptorPool.h
)

# Target
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
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
        INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "TRUE"
)

# Compile definitions
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

# Compile and link options
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
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:
            -O3;
            -march=native
            >
            $<$<CONFIG:Debug>:
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


# Dependencies
find_package(Vulkan REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw Vulkan::Vulkan)
target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/extern/glm
        ${CMAKE_SOURCE_DIR}/extern/spdlog/include
        ${CMAKE_SOURCE_DIR}/extern/imgui
        ${CMAKE_SOURCE_DIR}/extern/imgui/backends)
add_dependencies(${PROJECT_NAME} spdlog)

