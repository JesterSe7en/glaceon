#ifndef GLACEON_VULKANCONTEXT_H
#define GLACEON_VULKANCONTEXT_H

#include <vulkan/vulkan.h>

#include "VulkanBackend.h"
#include "VulkanDevice.h"

namespace Glaceon {

class VulkanContext {
 public:
  VulkanContext();
  ~VulkanContext() = default;

  void AddInstanceExtension(const char* extension);
  void RemoveInstanceExtension(const char* extension);

  void AddDeviceExtension(const char* extension);
  void RemoveDeviceExtension(const char* extension);

  VulkanBackend GetVulkanBackend() const { return backend; }
  VkInstance GetVulkanInstance() const { return instance; }
  void SetVulkanInstance(VkInstance vkInstance) { this->instance = vkInstance; }
  VulkanDevice GetVulkanDevice() const { return device; }

  std::vector<const char*> GetInstanceExtensions() const { return instanceExtensions; }
  std::vector<const char*> GetDeviceExtensions() const { return deviceExtensions; }

 private:
  VkInstance instance = VK_NULL_HANDLE;
  VulkanBackend backend;
  VulkanDevice device;

  std::vector<const char*> instanceExtensions;
  std::vector<const char*> deviceExtensions;
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANCONTEXT_H
