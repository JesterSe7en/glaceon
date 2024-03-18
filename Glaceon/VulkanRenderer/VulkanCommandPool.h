#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

class VulkanCommandPool {
 public:
  VulkanCommandPool(VulkanContext& context);

  void Initialize();
  void ResetCommandPool();
  void RebuildCommandBuffers();
  void Destroy();

  VkCommandPool GetVkCommandPool() const { return vk_command_pool_; }
  VkCommandBuffer* GetMainCommandBuffer() { return &vk_main_command_buffer_; }
  std::vector<VkCommandBuffer> GetFrameCommandBuffers() { return vk_frame_command_buffers_; }

 private:
  VulkanContext& context;

  VkCommandPool vk_command_pool_;
  VkCommandBuffer vk_main_command_buffer_;  // main command buffer for the engine; each frame buffer will have its own
  std::vector<VkCommandBuffer> vk_frame_command_buffers_;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_
