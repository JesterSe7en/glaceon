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
  void Rebuild();
  void Destroy();

  [[nodiscard]] const vk::PipelineLayout &GetVkPipelineLayout() const { return vk_pipeline_layout_; }
  [[nodiscard]] const vk::Pipeline &GetVkPipeline() const { return vk_pipeline_; }
  [[nodiscard]] const vk::PipelineCache &GetVkPipelineCache() const { return vk_pipeline_cache_; }

 private:
  VulkanContext &context_;
  GraphicsPipelineConfig pipeline_config_;

  vk::PipelineLayout vk_pipeline_layout_;
  vk::Pipeline vk_pipeline_;
  vk::PipelineCache vk_pipeline_cache_;

 private:
  void CreatePipelineLayout();
  vk::VertexInputBindingDescription GetPosColorBindingDescription();
  std::vector<vk::VertexInputAttributeDescription> GetPosColorAttributeDescriptions();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
