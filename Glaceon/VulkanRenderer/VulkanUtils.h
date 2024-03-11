//
// Created by alyxc on 3/10/2024.
//

#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_

#include "../pch.h"

namespace Glaceon {

class VulkanUtils {
 public:
  static std::vector<char> ReadFile(const std::string &filename);
  static VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<char> &code);
  static VkShaderModule CreateShaderModule(const VkDevice &device, const std::string &filename);
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
