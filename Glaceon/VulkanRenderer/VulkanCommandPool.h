#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_

#include "../pch.h"

namespace glaceon {

class VulkanContext;

class VulkanCommandPool {
 public:
  explicit VulkanCommandPool(VulkanContext &context);

  void Initialize();
  void ResetCommandPool();
  void RebuildCommandBuffers();
  void Destroy();

  [[nodiscard]] const vk::CommandPool &GetVkCommandPool() const { return vk_command_pool_; }
  [[nodiscard]] const vk::CommandBuffer &GetVkMainCommandBuffer() const { return vk_main_command_buffer_; }
  [[nodiscard]] const std::vector<vk::CommandBuffer> &GetVkFrameCommandBuffers() const {
    return vk_frame_command_buffers_;
  }

 private:
  VulkanContext &context_;

  vk::CommandPool vk_command_pool_;
  vk::CommandBuffer vk_main_command_buffer_;// main command buffer for the engine; each frame buffer will have its own
  std::vector<vk::CommandBuffer> vk_frame_command_buffers_;
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANCOMMANDPOOL_H_
