#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_

#include "../pch.h"

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
  };
  static Buffer CreateBuffer(BufferInputParams params);
  static int FindMemoryIndex(BufferInputParams params, vk::Buffer buffer);
  static void DestroyBuffer(BufferInputParams params, Buffer &buffer);
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
