cmake_minimum_required(VERSION 3.21)
set(PROJECT_NAME Glaceon)

project(${PROJECT_NAME})

# Source groups
set(Header_Files
    Glaceon.h
    pch.h
    Core/Base.h
    Core/Logger.h
    Core/Memory/Interface_Allocator.h
    Application.h
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
    VulkanRenderer/VulkanBase.h
    VulkanRenderer/VulkanMemoryAllocator.h
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
    Glaceon.cpp
    pch.cpp
    Core/Logger.cpp
    Application.cpp
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
    VulkanRenderer/VulkanMemoryAllocator.cpp
)
source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
)

# Target
add_library(${PROJECT_NAME} SHARED ${ALL_FILES})

# Precompiled headers
target_precompile_headers(${PROJECT_NAME} PRIVATE
                          "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/pch.h>"
)

# Set properties
set_target_properties(${PROJECT_NAME} PROPERTIES
                      VS_GLOBAL_KEYWORD "Win32Proj"
                      INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
                      INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO "TRUE"
)

# Compile definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
                           "$<$<CONFIG:Debug>:_DEBUG>"
                           "$<$<CONFIG:Release>:NDEBUG>"
                           "$<$<CONFIG:RelWithDebInfo>:NDEBUG>"
                           "GLACEON_EXPORTS"
                           "_WINDOWS"
                           "_USRDLL"
                           "UNICODE"
                           "_UNICODE"
)

# Compile and link options
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE
                           $<$<CONFIG:Release>:-O3;-march=native>
                           $<$<CONFIG:Debug>:-g;-Wall;-Wextra;-pedantic>
                           $<$<CONFIG:RelWithDebInfo>:-O2;-g;-march=native>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
                        $<$<CONFIG:Release>:-Wl,--strip-all>
                        $<$<CONFIG:RelWithDebInfo>:-Wl,--strip-all>
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

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
                      glm::glm
                      Vulkan::Headers
                      GPUOpen::VulkanMemoryAllocator
                      assimp::assimp
                      spdlog::spdlog
                      glfw
                      Vulkan::Vulkan
                      imgui::imgui
)
# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
                           ${Stb_INCLUDE_DIR}
)
