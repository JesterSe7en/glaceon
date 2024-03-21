#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_

#include "../pch.h"

namespace glaceon {

class VulkanUtils {
 public:
  static std::vector<char> ReadFile(const std::string &filename);
  static vk::ShaderModule CreateShaderModule(const vk::Device &device, const std::vector<char> &code);
  static vk::ShaderModule CreateShaderModule(const vk::Device &device, const std::string &filename);
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
