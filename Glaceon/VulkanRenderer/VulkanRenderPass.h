#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

class VulkanRenderPass {
 public:
  VulkanRenderPass(VulkanContext& context);
  void Initialize();
  void Destroy();

 private:
  VulkanContext& context;
  VkRenderPass renderPass;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANRENDERPASS_H_
