#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_

namespace Glaceon {

class VulkanContext;

struct GraphicsPipelineConfig {};

struct GraphicsPipelineOutBundle {};

class VulkanPipeline {
 public:
  VulkanPipeline(VulkanContext& context);
  void Initialize(GraphicsPipelineConfig pipelineConfig);
  void Recreate();
  void Destroy();

 private:
  VulkanContext& context;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
