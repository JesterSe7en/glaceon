#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_

#include "../pch.h"

namespace glaceon {

class VulkanContext;

struct GraphicsPipelineConfig {
  std::string vertex_shader_file;
  std::string fragment_shader_file;
};

class VulkanPipeline {
 public:
  explicit VulkanPipeline(VulkanContext &context);
  void Initialize(const GraphicsPipelineConfig &pipeline_config);

  [[nodiscard]] const vk::PipelineLayout &GetVkPipelineLayout() const { return vk_pipeline_layout_; }
  [[nodiscard]] const vk::Pipeline &GetVkPipeline() const { return vk_pipeline_; }
  [[nodiscard]] const vk::PipelineCache &GetVkPipelineCache() const { return vk_pipeline_cache_; }

  void Recreate();
  void Destroy();

 private:
  VulkanContext &context_;

  vk::PipelineLayout vk_pipeline_layout_;
  vk::Pipeline vk_pipeline_;
  vk::PipelineCache vk_pipeline_cache_;

 private:
  void CreatePipelineLayout();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
