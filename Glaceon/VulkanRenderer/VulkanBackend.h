#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

class VulkanBackend {
 public:
  explicit VulkanBackend(VulkanContext& context);
  void Initialize();
  void Destroy();

  [[nodiscard]] const vk::Instance& GetVkInstance() const { return instance_; }

 private:
  VulkanContext& context;
  vk::Instance instance_;

  static bool IsLayerAvailable(const std::vector<vk::LayerProperties>& layers, const char* layerToCheck);
  static bool IsExtensionAvailable(const std::vector<vk::ExtensionProperties>& allExtensions,
                                   const char* extensionToCheck);
  static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                VkDebugUtilsMessageTypeFlagsEXT messageType,
                                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_
