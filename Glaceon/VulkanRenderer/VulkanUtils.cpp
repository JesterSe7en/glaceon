//
// Created by alyxc on 3/10/2024.
//

#include "VulkanUtils.h"

namespace Glaceon {
std::vector<char> VulkanUtils::ReadFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::ate | std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + filename);
  }

  size_t fileSize = (size_t) file.tellg();
  std::vector<char> buffer(fileSize);

  file.seekg(0);
  file.read(buffer.data(), fileSize);

  file.close();
  return buffer;
}

VkShaderModule VulkanUtils::CreateShaderModule(VkDevice const& device, const std::vector<char>& code) {
  VkShaderModuleCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

  VkShaderModule shaderModule;
  if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create shader module.");
  }
  return shaderModule;
}

VkShaderModule VulkanUtils::CreateShaderModule(VkDevice const& device, const std::string& filename) {
  std::vector<char> code = ReadFile(filename);
  return CreateShaderModule(device, code);
}

}  // namespace Glaceon