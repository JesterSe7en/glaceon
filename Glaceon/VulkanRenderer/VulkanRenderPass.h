#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_

#include "../pch.h"

namespace glaceon {
class VulkanContext;

struct VulkanRenderPassInput {
  vk::Format depthFormat;
  vk::Format swapChainFormat;
};

class VulkanRenderPass {
 public:
  explicit VulkanRenderPass(VulkanContext& context);
  ~VulkanRenderPass();

  void Initialize(const VulkanRenderPassInput& input);
  void Rebuild();
  void Destroy();

  [[nodiscard]] const vk::RenderPass& GetVkRenderPass() const { return vk_render_pass_; }

 private:
  VulkanContext& context_;
  VulkanRenderPassInput input_{};
  vk::RenderPass vk_render_pass_;
};
}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_
