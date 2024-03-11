#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

struct GraphicsPipelineConfig {
  std::string vertexShaderFile;
  std::string fragmentShaderFile;
};

struct GraphicsPipelineOutBundle {};

class VulkanPipeline {
 public:
  VulkanPipeline(VulkanContext& context);
  void Initialize(GraphicsPipelineConfig pipelineConfig);

  void Recreate();
  void Destroy();

 private:
  VulkanContext& context;

  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;
  VkRenderPass renderPass;

  void CreatePipelineLayout();
  void CreateRenderPass();
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
