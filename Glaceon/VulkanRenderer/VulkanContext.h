#ifndef GLACEON_VULKANCONTEXT_H
#define GLACEON_VULKANCONTEXT_H

#include <vulkan/vulkan.h>

#include <utility>

#include "../Logger.h"
#include "VulkanBackend.h"
#include "VulkanDevice.h"

namespace Glaceon {

class VulkanContext {
 public:
  VulkanContext();
  ~VulkanContext() = default;

  VulkanBackend &GetVulkanBackend() { return backend; }
  VkInstance &GetVulkanInstance() { return instance; }
  void SetVulkanInstance(VkInstance vkInstance) { this->instance = vkInstance; }
  VulkanDevice &GetVulkanDevice() { return device; }

  void AddDeviceExtension(const char *ext);
  void RemoveDeviceExtension(const char *ext);
  std::vector<const char *> &GetDeviceExtensions() { return deviceExtensions; }
  void AddInstanceExtension(const char *ext);
  void RemoveInstanceExtension(const char *ext);
  std::vector<const char *> &GetInstanceExtensions() { return instanceExtensions; }

 private:
  VkInstance instance = VK_NULL_HANDLE;
  VulkanBackend backend;
  VulkanDevice device;

  std::vector<const char *> deviceExtensions;
  std::vector<const char *> instanceExtensions;

  std::string testString;
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANCONTEXT_H
