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
 private:
  VulkanContext &context;
  std::vector<VkQueueFamilyProperties> queue_family;
  bool CheckDeviceRequirements(VkPhysicalDevice& physicalDevice);
};


}  // namespace Glaceon

#endif  // GLACEON_VULKANDEVICE_H
