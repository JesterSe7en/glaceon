#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANPIPELINE_H_

#include "../pch.h"

namespace glaceon {

class VulkanContext;

struct GraphicsPipelineConfig {
  std::string vertex_shader_file;
  std::string fragment_shader_file;
};

struct DescriptorSetLayoutParams {
  int count;
  std::vector<int> binding;            // or indices the binding is describing
  std::vector<vk::DescriptorType> type;// e.g. uniform buffer or storage buffer, etc.
      // see https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkDescriptorType.html
  std::vector<int> type_count;               // number of descriptors of each type
  std::vector<vk::ShaderStageFlagBits> stage;// stage to bind to
};

class VulkanPipeline {
 public:
  explicit VulkanPipeline(VulkanContext &context);
  void CreateDescriptorSetLayout(DescriptorSetLayoutParams params);
  void Initialize(const GraphicsPipelineConfig &pipeline_config);
  void Rebuild();
  void Destroy();

  [[nodiscard]] const vk::PipelineLayout &GetVkPipelineLayout() const { return vk_pipeline_layout_; }
  [[nodiscard]] const vk::Pipeline &GetVkPipeline() const { return vk_pipeline_; }
  [[nodiscard]] const vk::PipelineCache &GetVkPipelineCache() const { return vk_pipeline_cache_; }
  [[nodiscard]] const vk::DescriptorSetLayout &GetVkDescriptorSetLayout() const { return vk_descriptor_set_layout_; }


 private:
  VulkanContext &context_;
  GraphicsPipelineConfig pipeline_config_;

  vk::DescriptorSetLayout vk_descriptor_set_layout_;

 private:
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
