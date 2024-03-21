#include "VulkanPipeline.h"

#include "../Logger.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace glaceon {

VulkanPipeline::VulkanPipeline(VulkanContext &context) : context_(context) {}

void VulkanPipeline::Initialize(const GraphicsPipelineConfig &pipeline_config) {
  vk::Device device = context_.GetVulkanLogicalDevice();
  assert(device != nullptr);

  vk::PipelineCacheCreateInfo pipeline_cache_info;
  pipeline_cache_info.sType = vk::StructureType::ePipelineCacheCreateInfo;
  pipeline_cache_info.pNext = nullptr;
  pipeline_cache_info.flags = vk::PipelineCacheCreateFlags();
  pipeline_cache_info.initialDataSize = 0;
  pipeline_cache_info.pInitialData = nullptr;
  if (device.createPipelineCache(&pipeline_cache_info, nullptr, &vk_pipeline_cache_) != vk::Result::eSuccess) {
    GERROR("Failed to create pipeline cache");
    return;
  }

  vk::GraphicsPipelineCreateInfo pipeline_create_info = {};
  pipeline_create_info.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
  pipeline_create_info.pNext = nullptr;
  pipeline_create_info.flags = vk::PipelineCreateFlags();

  std::vector<vk::PipelineShaderStageCreateInfo>
      shader_stages;// these store the configurations for vertex input, fragment, etc.

  // Vertex Input
  vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
  vertex_input_info.flags = vk::PipelineVertexInputStateCreateFlags();
  vertex_input_info.pNext = nullptr;
  vertex_input_info.vertexAttributeDescriptionCount = 0;
  vertex_input_info.pVertexAttributeDescriptions = nullptr;
  vertex_input_info.vertexBindingDescriptionCount = 0;
  vertex_input_info.pVertexBindingDescriptions = nullptr;
  pipeline_create_info.pVertexInputState = &vertex_input_info;

  // Input Assembly - What sorts of shapes are we creating?
  vk::PipelineInputAssemblyStateCreateInfo input_assembly_info = {};
  input_assembly_info.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
  input_assembly_info.topology = vk::PrimitiveTopology::eTriangleList;
  pipeline_create_info.pInputAssemblyState = &input_assembly_info;

  // Vertex Shader
  if (pipeline_config.vertex_shader_file.empty()) {
    GERROR("No vertex shader specified");
    return;
  }

  vk::ShaderModule vertex_shader = VulkanUtils::CreateShaderModule(device, pipeline_config.vertex_shader_file);
  if (vertex_shader == nullptr) {
    GERROR("Failed to create vertex shader module");
    return;
  } else {
    GINFO("Successfully created vertex shader module");
  }

  vk::PipelineShaderStageCreateInfo vertex_shader_stage = {};
  vertex_shader_stage.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  vertex_shader_stage.stage = vk::ShaderStageFlagBits::eVertex;
  vertex_shader_stage.module = vertex_shader;
  vertex_shader_stage.pName = "main";
  shader_stages.push_back(vertex_shader_stage);

  // Viewport and Scissor
  vk::Extent2D extent = context_.GetVulkanSwapChain().GetSwapChainExtent();
  vk::Viewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  vk::Rect2D scissor = {};
  scissor.offset = vk::Offset2D{0, 0};
  scissor.extent = extent;

  vk::PipelineViewportStateCreateInfo viewport_state = {};
  viewport_state.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
  viewport_state.flags = vk::PipelineViewportStateCreateFlags();
  viewport_state.viewportCount = 1;
  viewport_state.pViewports = &viewport;
  viewport_state.scissorCount = 1;
  viewport_state.pScissors = &scissor;
  pipeline_create_info.pViewportState = &viewport_state;

  // Rasteurizer - fills in all the fragments aka pixels
  vk::PipelineRasterizationStateCreateInfo rasterizer_info = {};
  rasterizer_info.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
  rasterizer_info.pNext = nullptr;
  rasterizer_info.flags = vk::PipelineRasterizationStateCreateFlags();
  rasterizer_info.depthClampEnable = VK_FALSE;
  rasterizer_info.rasterizerDiscardEnable = VK_FALSE;
  rasterizer_info.polygonMode = vk::PolygonMode::eFill;
  rasterizer_info.lineWidth = 1.0f;
  rasterizer_info.cullMode = vk::CullModeFlagBits::eBack;
  rasterizer_info.frontFace = vk::FrontFace::eClockwise;
  rasterizer_info.depthBiasEnable = VK_FALSE;
  pipeline_create_info.pRasterizationState = &rasterizer_info;

  // Fragment shader
  if (pipeline_config.vertex_shader_file.empty()) {
    GERROR("No fragment shader specified");
    return;
  }

  vk::ShaderModule fragment_shader = VulkanUtils::CreateShaderModule(device, pipeline_config.fragment_shader_file);
  if (fragment_shader == nullptr) {
    GERROR("Failed to create fragment shader module");
    return;
  } else {
    GINFO("Successfully created fragment shader module");
  }

  vk::PipelineShaderStageCreateInfo fragment_shader_stage = {};
  fragment_shader_stage.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  fragment_shader_stage.stage = vk::ShaderStageFlagBits::eFragment;
  fragment_shader_stage.module = fragment_shader;
  fragment_shader_stage.pName = "main";
  shader_stages.push_back(fragment_shader_stage);

  // Adding shader modules to pipeline
  pipeline_create_info.stageCount = shader_stages.size();
  pipeline_create_info.pStages = shader_stages.data();

  // Multisampling
  vk::PipelineMultisampleStateCreateInfo multisampling_info = {};
  multisampling_info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
  pipeline_create_info.pMultisampleState = &multisampling_info;

  // Color Blend - alpha transparency, etc.
  // This is doing no blending
  vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask =
      vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
                              | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
  color_blend_attachment.blendEnable = VK_FALSE;

  vk::PipelineColorBlendStateCreateInfo color_blending = {};
  color_blending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
  color_blending.logicOp = vk::LogicOp::eCopy;
  color_blending.attachmentCount = 1;
  color_blending.pAttachments = &color_blend_attachment;
  color_blending.blendConstants[0] = 0.0f;
  color_blending.blendConstants[1] = 0.0f;
  color_blending.blendConstants[2] = 0.0f;
  color_blending.blendConstants[3] = 0.0f;
  pipeline_create_info.pColorBlendState = &color_blending;

  // Pipeline layout
  CreatePipelineLayout();
  pipeline_create_info.layout = vk_pipeline_layout_;

  // Render Pass
  vk::RenderPass render_pass = context_.GetVulkanRenderPass().GetVkRenderPass();
  assert(render_pass != nullptr);
  pipeline_create_info.renderPass = render_pass;

  // Extra stuff
  pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;// Optional - to base pipeline on

  // Create pipeline - for now just one pipeline
  if (device.createGraphicsPipelines(vk_pipeline_cache_, 1, &pipeline_create_info, nullptr, &vk_pipeline_)
      != vk::Result::eSuccess) {
    GERROR("Failed to create graphics pipeline");
    vk_pipeline_ = nullptr;
  } else {
    GINFO("Successfully created graphics pipeline");
  }

  // Clean up shader modules
  vkDestroyShaderModule(device, vertex_shader, nullptr);
  vkDestroyShaderModule(device, fragment_shader, nullptr);
}

// This is so we can push constants and descriptor sets aka. Uniforms
void VulkanPipeline::CreatePipelineLayout() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  assert(device != nullptr);

  vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
  // here we are not setting ANY uniform data
  pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  pipeline_layout_info.setLayoutCount = 0;// this can push arbitrary data (usually large like an image) to pipeline
  pipeline_layout_info.pSetLayouts = nullptr;

  //  pipeline_layout_info.pushConstantRangeCount = 0;  // this can only push small data to pipeline like one matrix
  //  pipeline_layout_info.pPushConstantRanges = nullptr;

  pipeline_layout_info.pushConstantRangeCount = 1;
  vk::PushConstantRange push_constant_info = {};
  // pushing one matrix to pipeline as a "global" uniform
  push_constant_info.stageFlags = vk::ShaderStageFlagBits::eVertex;
  push_constant_info.offset = 0;
  push_constant_info.size = sizeof(glm::mat4);
  pipeline_layout_info.pPushConstantRanges = &push_constant_info;

  if (device.createPipelineLayout(&pipeline_layout_info, nullptr, &vk_pipeline_layout_) != vk::Result::eSuccess) {
    GERROR("Failed to create pipeline layout");
    vk_pipeline_layout_ = nullptr;
  } else {
    GINFO("Successfully created pipeline layout");
  }
}

void VulkanPipeline::Destroy() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  if (vk_pipeline_cache_ != VK_NULL_HANDLE) {
    device.destroy(vk_pipeline_cache_, nullptr);
    vk_pipeline_cache_ = VK_NULL_HANDLE;
  }

  if (vk_pipeline_layout_ != VK_NULL_HANDLE) {
    device.destroy(vk_pipeline_layout_, nullptr);
    vk_pipeline_layout_ = VK_NULL_HANDLE;
  }

  if (vk_pipeline_ != VK_NULL_HANDLE) {
    device.destroy(vk_pipeline_, nullptr);
    vk_pipeline_ = VK_NULL_HANDLE;
  }
}

}// namespace glaceon
