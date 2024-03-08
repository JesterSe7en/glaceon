#ifndef GLACEON_VULKANCONTEXT_H
#define GLACEON_VULKANCONTEXT_H

#include "../pch.h"

#include "VulkanBackend.h"
#include "VulkanDevice.h"
#include "VulkanSwapChain.h"

namespace Glaceon {

class VulkanContext {
 public:
  VulkanContext();
  ~VulkanContext() = default;

  VulkanBackend &GetVulkanBackend() { return backend; }

  VkInstance &GetVulkanInstance() { return instance; }
  void SetVulkanInstance(VkInstance vkInstance) { this->instance = vkInstance; }

  VulkanDevice &GetVulkanDevice() { return device; }
  VkPhysicalDevice GetVulkanPhysicalDevice() { return device.GetPhysicalDevice(); }
  VkDevice GetVulkanLogicalDevice() { return device.GetLogicalDevice(); }
  QueueIndexes &GetQueueIndexes() { return device.GetQueueIndexes(); }

  void AddDeviceExtension(const char *ext);
  void RemoveDeviceExtension(const char *ext);
  std::vector<const char *> &GetDeviceExtensions() { return deviceExtensions; }
  void AddInstanceExtension(const char *ext);
  void RemoveInstanceExtension(const char *ext);
  std::vector<const char *> &GetInstanceExtensions() { return instanceExtensions; }

  VkDescriptorPool GetDescriptorPool() { return descriptorPool; }
  void SetDescriptorPool(VkDescriptorPool vkDescriptorPool) { this->descriptorPool = vkDescriptorPool; }

  VkPipelineCache GetPipelineCache() { return pipelineCache; }
  void SetPipelineCache(VkPipelineCache vkPipelineCache) { this->pipelineCache = vkPipelineCache; }

  VkSurfaceKHR GetSurface() { return surface; }
  void SetSurface(VkSurfaceKHR vkSurface) { this->surface = vkSurface; }

  VulkanSwapChain &GetVulkanSwapChain() { return swapChain; }

 private:
  VkInstance instance = VK_NULL_HANDLE;
  VulkanBackend backend;
  VulkanDevice device;
  VulkanSwapChain swapChain;
  VkDescriptorPool descriptorPool;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;

  std::vector<const char *> deviceExtensions;
  std::vector<const char *> instanceExtensions;
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANCONTEXT_H
