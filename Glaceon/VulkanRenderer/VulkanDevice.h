#ifndef GLACEON_VULKANDEVICE_H
#define GLACEON_VULKANDEVICE_H

#include <vulkan/vulkan_core.h>

#include <vector>

namespace Glaceon {

class VulkanContext;

class VulkanDevice {
 public:
  VulkanDevice(VulkanContext &context);
  void Initialize();

  std::vector<VkQueueFamilyProperties> GetQueueFamilies() { return queueFamily; }

 private:
  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  VulkanContext &context;
  std::vector<VkQueueFamilyProperties> queueFamily;
  std::vector<VkExtensionProperties> deviceExtensions;
  bool CheckDeviceRequirements(VkPhysicalDevice &vkPhysicalDevice);
  bool IsExtensionAvailable(VkPhysicalDeviceProperties properties, const char *ext);
  uint32_t GetQueueFamilyIndex(VkQueueFlagBits bits);

  static void PrintPhysicalDevice(VkPhysicalDevice gpu);
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANDEVICE_H
