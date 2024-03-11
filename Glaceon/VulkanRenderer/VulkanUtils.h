//
// Created by alyxc on 3/10/2024.
//

#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_

namespace Glaceon {

class VulkanUtils {
  static std::vector<char> ReadFile(const std::string &filename);
  static VkShaderModule CreateShaderModule(const VkDevice &device, const std::vector<char> &code);
  static VkShaderModule CreateShaderModule(const VkDevice &device, const std::string &filename);
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANUTILS_H_
