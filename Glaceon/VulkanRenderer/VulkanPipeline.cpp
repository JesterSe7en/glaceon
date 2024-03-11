//
// Created by alyxc on 3/10/2024.
//

#include "VulkanPipeline.h"

#include "VulkanContext.h"

namespace Glaceon {
VulkanPipeline::VulkanPipeline(VulkanContext& context) :context(context){

}
void VulkanPipeline::Initialize(GraphicsPipelineConfig pipelineConfig) {
  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;

}
}  // namespace Glaceon