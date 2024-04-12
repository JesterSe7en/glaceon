#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_

#include "../pch.h"
#include <vulkan/vulkan_handles.hpp>

namespace glaceon {

class VulkanUtils {
 public:
  static std::vector<char> ReadFile(const std::string &filename);
  static vk::ShaderModule CreateShaderModule(const vk::Device &device, const std::vector<char> &code);
  static vk::ShaderModule CreateShaderModule(const vk::Device &device, const std::string &filename);

  // Buffer management

  struct Buffer {
    vk::Buffer buffer;
    vk::DeviceMemory buffer_memory;
  };

  // input struct
  struct BufferInputParams {
    vk::Device device;
    vk::PhysicalDevice physical_device;
    size_t size;
    vk::BufferUsageFlags buffer_usage;
    vk::MemoryPropertyFlags memory_property_flags;// these are the types of memory that you want to use on the GPU
  };
  static Buffer CreateBuffer(BufferInputParams params);
  static int FindMemoryIndex(BufferInputParams &params, vk::Buffer buffer);
  static uint32_t FindMemoryTypeIndex(vk::PhysicalDevice physical_device, uint32_t type_filter, vk::MemoryPropertyFlags properties);
  static void CopyBuffer(Buffer &src, Buffer &dst, vk::DeviceSize size, vk::Queue queue, vk::CommandBuffer command_buffer);
  static void DestroyBuffer(BufferInputParams params, Buffer &buffer);

  // Job management
  static void BeginSingleTimeCommands(vk::CommandBuffer command_buffer);
  static void EndSingleTimeCommands(vk::CommandBuffer command_buffer, vk::Queue queue);

  // Image formatting
  static vk::Format FindSupportedFormat(vk::PhysicalDevice physical_device, std::vector<vk::Format> candidates, vk::ImageTiling tiling,
                                        vk::FormatFeatureFlags features);
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
