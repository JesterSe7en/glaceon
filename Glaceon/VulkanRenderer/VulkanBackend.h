#ifndef GLACEON_VULKANBACKEND_H
#define GLACEON_VULKANBACKEND_H

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

class VulkanBackend {
 public:
  VulkanBackend(VulkanContext& context);
  void Initialize();
  void Destroy();

  VkInstance& GetVkInstance() { return vkInstance; }

 private:
  VulkanContext& context;

  VkInstance vkInstance;
  std::vector<VkExtensionProperties> extensions;

  void PopulateInstanceExtensions();

  static bool IsLayerAvailable(const std::vector<VkLayerProperties>& layerProperties, const char* layerName);
  static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& extensions, const char* extension);
  static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANBACKEND_H
