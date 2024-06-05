set(PROJECT_NAME Glaceon)

# Source groups
set(Header_Files
    Glaceon.h
    pch.h
    Core/Base.h
    Core/Logger.h
    Core/Memory/Interface_Allocator.h
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
    VulkanRenderer/VulkanDescriptorPool.h
    VulkanRenderer/VulkanTexture.h
    TriangleMesh.h
    SquareMesh.h
    StarMesh.h
    VertexBufferCollection.h
    Scene.h
    Assimp/AssimpImporter.h
    Utils.h
    Core/Memory/MemorySubsystem.h
    Core/Memory/StackAllocator.h
    Core/Memory/FreeListAllocator.h
    Core/Memory/RingAllocator.h
    Profiler/InstrumentationTimer.h
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    Glaceon.cpp
    pch.cpp
    Core/Logger.cpp
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
    VulkanRenderer/VulkanDescriptorPool.cpp
    VulkanRenderer/VulkanTexture.cpp
    VulkanRenderer/VulkanSync.cpp
    TriangleMesh.cpp
    SquareMesh.cpp
    StarMesh.cpp
    VertexBufferCollection.cpp
    Scene.cpp
    Core/Memory/PoolAllocator.cpp
    Core/Memory/RingAllocator.cpp
    Core/Memory/FreeListAllocator.cpp
    Core/Memory/StackAllocator.cpp
    Core/Memory/LinearAllocator.cpp
    Core/Memory/MemorySubsystem.cpp
    Assimp/AssimpImporter.cpp
    Utils.cpp
    Profiler/InstrumentationTimer.cpp
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    VulkanRenderer/VulkanBase.h
    VulkanRenderer/VulkanMemoryAllocator.cpp
    VulkanRenderer/VulkanMemoryAllocator.h

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
# if (MSVC)
#     target_compile_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Release>:
#             /Oi;
#             /Gy
#             >
#             $<$<CONFIG:RelWithDebInfo>:
#             /Oi;
#             /Gy
#             >
#             /permissive-;
#             /sdl;
#             /W3;
#             ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
#             ${DEFAULT_CXX_EXCEPTION_HANDLING}
#     )
#     target_link_options(${PROJECT_NAME} PRIVATE
#             $<$<CONFIG:Release>:
#             /OPT:REF;
#             /OPT:ICF
#             >
#             $<$<CONFIG:RelWithDebInfo>:
#             /OPT:REF;
#             /OPT:ICF
#             >
#             /DEBUG;
#             /SUBSYSTEM:WINDOWS
#     )
# else
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
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
endif()


# Dependencies
find_package(Vulkan REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(assimp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE
                      glm::glm
                      Vulkan::Headers
                      GPUOpen::VulkanMemoryAllocator
                      assimp::assimp
                      spdlog::spdlog
                      glfw
                      Vulkan::Vulkan
                      imgui::imgui
)

target_include_directories(${PROJECT_NAME} PRIVATE
                           ${Stb_INCLUDE_DIR}
)
