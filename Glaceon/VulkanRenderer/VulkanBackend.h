#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_

#include "../pch.h"

namespace glaceon {

class VulkanContext;

class VulkanBackend {
 public:
  explicit VulkanBackend(VulkanContext &context);
  void Initialize();
  void Destroy();

  [[nodiscard]] const vk::Instance &GetVkInstance() const { return instance_; }

 private:
  VulkanContext &context_;
  vk::Instance instance_;

  static bool IsLayerAvailable(const std::vector<vk::LayerProperties> &layers, const char *layer_to_check);
  static bool IsExtensionAvailable(const std::vector<vk::ExtensionProperties> &all_extensions,
                                   const char *extension_to_check);
  static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                VkDebugUtilsMessageTypeFlagsEXT message_type,
                                const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, void *p_user_data);
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANBACKEND_H_
