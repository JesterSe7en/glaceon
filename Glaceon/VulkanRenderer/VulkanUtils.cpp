#include "VulkanUtils.h"
#include "../Logger.h"

namespace glaceon {
std::vector<char> VulkanUtils::ReadFile(const std::string &filename) {
  std::vector<char> buffer;
  try {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      GERROR("Failed to open file: {}", filename);
      return buffer;
    }

    size_t file_size = (size_t) file.tellg();
    buffer.resize(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
  } catch (std::exception &e) {
    GERROR("Exception caught while reading file: {}", e.what());
    buffer.clear();
  }
  return buffer;
}

vk::ShaderModule VulkanUtils::CreateShaderModule(vk::Device const &device, const std::vector<char> &code) {
  vk::ShaderModule shader_module = VK_NULL_HANDLE;
  try {
    if (code.empty()) {
      GERROR("Cannot create shader module: Shader code is empty");
      return VK_NULL_HANDLE;
    }

    if (!device) {
      GERROR("Cannot create shader module: Device is null");
      return VK_NULL_HANDLE;
    }

    vk::ShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = vk::StructureType::eShaderModuleCreateInfo;
    shader_module_create_info.codeSize = code.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    if (device.createShaderModule(&shader_module_create_info, nullptr, &shader_module) != vk::Result::eSuccess) {
      GERROR("Failed to create shader module");
      return VK_NULL_HANDLE;
    }
  } catch (std::exception &e) {
    GERROR("Exception caught while creating shader module: {}", e.what());
    if (shader_module != VK_NULL_HANDLE) {
      device.destroy(shader_module);
      shader_module = VK_NULL_HANDLE;
    }
  }
  return shader_module;
}

vk::ShaderModule VulkanUtils::CreateShaderModule(vk::Device const &device, const std::string &filename) {
  std::vector<char> code = ReadFile(filename);
  if (code.empty()) { return VK_NULL_HANDLE; }
  return CreateShaderModule(device, code);
}

}// namespace glaceon
