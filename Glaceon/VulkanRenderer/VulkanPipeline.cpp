#include "VulkanPipeline.h"

#include <vulkan/vulkan_core.h>

#include "VulkanContext.h"

namespace Glaceon {

VulkanPipeline::VulkanPipeline(VulkanContext& context) : context(context) {}

void VulkanPipeline::Initialize(GraphicsPipelineConfig pipelineConfig) {
  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = nullptr;
  pipelineInfo.flags = 0;

  std::vector<VkPipelineShaderStageCreateFlags>
      shaderStages;  // these store the configurations for vertex input, fragment, etc.

  // Vertex Input
  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.flags = 0;
  vertexInputInfo.pNext = nullptr;
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr;
  pipelineInfo.pVertexInputState = &vertexInputInfo;

  // Input Assembly - What sort of shapes are we creating?
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
  inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

  // Fragment shader
}
}  // namespace Glaceon
