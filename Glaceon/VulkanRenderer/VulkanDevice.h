#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANDEVICE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANDEVICE_H_

#include "../pch.h"

namespace glaceon {

class VulkanContext;

struct QueueIndexes {
  std::optional<uint32_t> graphics_family = std::nullopt;
  std::optional<uint32_t> present_family = std::nullopt;

  [[nodiscard]] bool IsComplete() const { return graphics_family.has_value() && present_family.has_value(); }
};

class VulkanDevice {
 public:
  explicit VulkanDevice(VulkanContext &context);
  void Initialize();
  void Destroy();

  [[nodiscard]] const vk::PhysicalDevice &GetVkPhysicalDevice() const {
    return vk_physical_device_;
  }
  [[nodiscard]] const vk::Device &GetVkDevice() const {
    return vk_device_;
  }
  [[nodiscard]] const vk::Queue &GetVkPresentQueue() const {
    return vk_present_queue_;
  }
  [[nodiscard]] const vk::Queue &GetVkGraphicsQueue() const {
    return vk_graphics_queue_;
  }
  [[nodiscard]] const vk::CommandPool &GetVkCommandPool() const {
    return vk_command_pool_;
  }
  [[nodiscard]] const vk::DescriptorPool &GetVkDescriptorPool() const {
    return vk_descriptor_pool_;
  }

  QueueIndexes &GetQueueIndexes() { return queue_indexes_; }

 private:
  vk::PhysicalDevice vk_physical_device_;
  vk::Device vk_device_;
  vk::Queue vk_present_queue_;
  vk::Queue vk_graphics_queue_;
  vk::CommandPool vk_command_pool_;
  vk::DescriptorPool vk_descriptor_pool_;

  std::vector<vk::QueueFamilyProperties> queue_family_;
  std::vector<vk::ExtensionProperties> device_extensions_;

  VulkanContext &context_;

  QueueIndexes queue_indexes_;

  bool CheckDeviceRequirements(vk::PhysicalDevice &vk_physical_device);
  bool IsExtensionAvailable(const char *ext);
  static void PrintPhysicalDevice(vk::PhysicalDevice gpu);
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANDEVICE_H_
