#ifndef GLACEON_VULKANCONTEXT_H
#define GLACEON_VULKANCONTEXT_H

#include "../pch.h"
#include "VulkanBackend.h"
#include "VulkanCommandPool.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanSync.h"

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

  VulkanPipeline &GetVulkanPipeline() { return pipeline; }

  VulkanRenderPass &GetVulkanRenderPass() { return renderPass; }

  VulkanCommandPool &GetVulkanCommandPool() { return commandPool; }

  VulkanSync &GetVulkanSync() { return sync; }

  uint32_t currentFrameIndex = 0;
  uint32_t semaphoreIndex = 0;

 private:
  VulkanBackend backend;
  VulkanDevice device;
  VulkanSwapChain swapChain;
  VulkanRenderPass renderPass;
  VulkanPipeline pipeline;
  VulkanCommandPool commandPool;
  VulkanSync sync;

  uint32_t CurrentFrameIndex = 0;

  VkInstance instance = VK_NULL_HANDLE;
  VkDescriptorPool descriptorPool;
  VkSurfaceKHR surface = VK_NULL_HANDLE;
  VkPipelineCache pipelineCache = VK_NULL_HANDLE;

  std::vector<const char *> deviceExtensions;
  std::vector<const char *> instanceExtensions;
};

}  // namespace Glaceon

#endif  // GLACEON_VULKANCONTEXT_H
