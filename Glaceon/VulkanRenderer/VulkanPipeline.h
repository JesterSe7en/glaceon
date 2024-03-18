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

  VkPipelineCache GetPipelineCache() { return pipelineCache; }

  void Recreate();
  void Destroy();

 private:
  VulkanContext& context;

  VkPipelineLayout pipelineLayout;
  VkPipeline pipeline;
  VkPipelineCache pipelineCache;

 private:
  void CreatePipelineLayout();
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
