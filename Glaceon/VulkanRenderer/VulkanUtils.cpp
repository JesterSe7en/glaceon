#include "VulkanUtils.h"
#include "../Logger.h"

namespace Glaceon {
std::vector<char> VulkanUtils::ReadFile(const std::string& filename) {
  std::vector<char> buffer;
  try {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      GERROR("Failed to open file: {}", filename);
      return buffer;
    }

    size_t fileSize = (size_t)file.tellg();
    buffer.resize(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();
  } catch (std::exception& e) {
    GERROR("Exception caught while reading file: {}", e.what());
    buffer.clear();
  }
  return buffer;
}

VkShaderModule VulkanUtils::CreateShaderModule(VkDevice const& device, const std::vector<char>& code) {
  VkShaderModule shaderModule = VK_NULL_HANDLE;
  try {
    if (code.empty()) {
      GERROR("Cannot create shader module: Shader code is empty");
      return shaderModule;
    }

    if (!device) {
      GERROR("Cannot create shader module: Device is null");
      return shaderModule;
    }

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
      GERROR("Failed to create shader module");
      return shaderModule;
    }
  } catch (std::exception& e) {
    GERROR("Exception caught while creating shader module: {}", e.what());
    if (shaderModule != VK_NULL_HANDLE) {
      vkDestroyShaderModule(device, shaderModule, nullptr);
      shaderModule = VK_NULL_HANDLE;
    }
  }
  return shaderModule;
}

VkShaderModule VulkanUtils::CreateShaderModule(VkDevice const& device, const std::string& filename) {
  std::vector<char> code = ReadFile(filename);
  if (code.empty()) {
    return VK_NULL_HANDLE;
  }
  return CreateShaderModule(device, code);
}

}  // namespace Glaceon