#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANCONTEXT_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANCONTEXT_H_

#include "../pch.h"
#include "VulkanBackend.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanSync.h"

namespace glaceon {

class VulkanContext {
 public:
  VulkanContext();
  ~VulkanContext() = default;

  const vk::Instance &GetVulkanInstance() { return backend_.GetVkInstance(); }

  vk::PhysicalDevice GetVulkanPhysicalDevice() { return device_.GetVkPhysicalDevice(); }
  vk::Device GetVulkanLogicalDevice() { return device_.GetVkDevice(); }
  QueueIndexes &GetQueueIndexes() { return device_.GetQueueIndexes(); }

  void AddDeviceExtension(const char *ext);
  void RemoveDeviceExtension(const char *ext);
  std::vector<const char *> &GetDeviceExtensions() { return device_extensions_; }
  void AddInstanceExtension(const char *ext);
  void RemoveInstanceExtension(const char *ext);
  std::vector<const char *> &GetInstanceExtensions() { return instance_extensions_; }

  vk::SurfaceKHR GetSurface() { return surface_; }
  void SetSurface(vk::SurfaceKHR vk_surface) { this->surface_ = vk_surface; }

  VulkanBackend &GetVulkanBackend() { return backend_; }
  VulkanDevice &GetVulkanDevice() { return device_; }
  VulkanSwapChain &GetVulkanSwapChain() { return swap_chain_; }
  VulkanRenderPass &GetVulkanRenderPass() { return render_pass_; }
  VulkanPipeline &GetVulkanPipeline() { return pipeline_; }
  VulkanCommandPool &GetVulkanCommandPool() { return command_pool_; }
  VulkanDescriptorPool &GetVulkanDescriptorPool() { return descriptor_pool_; }
  VulkanSync &GetVulkanSync() { return sync_; }

  void Destroy();

  uint32_t current_frame_index_ = 0;
  uint32_t semaphore_index_ = 0;

 private:
  VulkanBackend backend_;
  VulkanDevice device_;
  VulkanSwapChain swap_chain_;
  VulkanRenderPass render_pass_;
  VulkanPipeline pipeline_;
  VulkanCommandPool command_pool_;
  VulkanDescriptorPool descriptor_pool_;

 private:
  VulkanSync sync_;

  vk::SurfaceKHR surface_ = VK_NULL_HANDLE;

  std::vector<const char *> device_extensions_;
  std::vector<const char *> instance_extensions_;
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANCONTEXT_H_
