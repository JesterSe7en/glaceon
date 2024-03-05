#ifndef GLACEON_VULKANBACKEND_H
#define GLACEON_VULKANBACKEND_H

#include <vulkan/vulkan.h>

#include <vector>

namespace Glaceon {

class VulkanContext;

class VulkanBackend {
 public:
  VulkanBackend(VulkanContext& context);
  void Initialize();

 private:
  VulkanContext& context;
  std::vector<VkExtensionProperties> extensions;

  bool IsLayerAvailable(const std::vector<VkLayerProperties>& layerProperties, const char* layerName);
  bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& extensions, const char* extension);
  void PopulateInstanceExtensions();

  static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANBACKEND_H
