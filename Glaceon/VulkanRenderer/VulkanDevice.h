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
  VkDevice device = VK_NULL_HANDLE;
  VkQueue queue;
  VulkanContext &context;
  std::vector<VkQueueFamilyProperties> queueFamily;
  std::vector<VkExtensionProperties> deviceExtensions;
  bool CheckDeviceRequirements(VkPhysicalDevice &vkPhysicalDevice);
  bool IsExtensionAvailable(VkPhysicalDeviceProperties properties, const char *ext);

  /**
   * Get the index of the queue family that supports the specified queue flags.
   * @param bits the queue flags to check
   * @return the index of the queue family that supports the specified queue flags, or -1 if not found
   * @throws None
   */
  int GetQueueFamilyIndex(VkQueueFlagBits bits);

  static void PrintPhysicalDevice(VkPhysicalDevice gpu);
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANDEVICE_H
