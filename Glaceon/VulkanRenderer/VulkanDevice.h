#ifndef GLACEON_VULKANDEVICE_H
#define GLACEON_VULKANDEVICE_H

#include <vulkan/vulkan_core.h>

#include <optional>
#include <vector>

namespace Glaceon {

class VulkanContext;

struct QueueIndexes {
  std::optional<uint32_t> graphicsFamily = std::nullopt;
  std::optional<uint32_t> presentFamily = std::nullopt;

  [[nodiscard]] bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class VulkanDevice {
 public:
  VulkanDevice(VulkanContext &context);
  void Initialize();

  VkPhysicalDevice &GetPhysicalDevice() { return physicalDevice; }
  VkDevice &GetLogicalDevice() { return device; }
  VkQueue GetPresentQueue() { return presentQueue; }
  VkQueue GetGraphicsQueue() { return graphicsQueue; }

  QueueIndexes &GetQueueIndexes() { return queue_indexes_; }

 private:
  VkPhysicalDevice physicalDevice;
  VkDevice device;
  VkQueue presentQueue;
  VkQueue graphicsQueue;
  VkDescriptorPool descriptorPool;
  VulkanContext &context;
  std::vector<VkQueueFamilyProperties> queueFamily;
  std::vector<VkExtensionProperties> deviceExtensions;

  QueueIndexes queue_indexes_;

  bool CheckDeviceRequirements(VkPhysicalDevice &vkPhysicalDevice);
  bool IsExtensionAvailable(const char *ext);

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
