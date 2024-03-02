#ifndef GLACEON_GLACEON_VULKANAPI_H_
#define GLACEON_GLACEON_VULKANAPI_H_

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#include <memory>
#include <vector>

namespace Glaceon {

class VulkanAPI {
 public:
  static void initVulkan(std::vector<const char *> instance_extensions);
  static VkInstance getVulkanInstance() { return vkInstance; }
  static VkPhysicalDevice getVulkanPhysicalDevice() { return vkPhysicalDevice; }
  static VkDevice getVulkanDevice() { return vkDevice; }
  static uint32_t getVulkanGraphicsQueueFamilyIndex() { return vkGraphicsQueueFamilyIndex; }
  static VkQueue getVulkanQueue() { return vkQueue; }
  static VkPipelineCache getVulkanPipelineCache() { return vkPipelineCache; }
  static VkDescriptorPool getVulkanDescriptorPool() { return vkDescriptorPool; }

 private:
  static VkInstance vkInstance;
  static VkPhysicalDevice vkPhysicalDevice;
  static uint32_t vkGraphicsQueueFamilyIndex;
  static VkDevice vkDevice;
  static VkQueue vkQueue;
  static VkDescriptorPool vkDescriptorPool;
  static VkPipelineCache vkPipelineCache;
  //  static std::shared_ptr<VkInstance> p_vkInstance;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANAPI_H_
