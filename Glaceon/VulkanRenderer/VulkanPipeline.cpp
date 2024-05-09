#include "VulkanPipeline.h"

#include "../Core/Base.h"
#include "../Core/Logger.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace glaceon {

VulkanPipeline::VulkanPipeline(VulkanContext &context)
    : context_(context),
      vk_pipeline_layout_(VK_NULL_HANDLE),
      vk_pipeline_(VK_NULL_HANDLE),
      vk_pipeline_cache_(VK_NULL_HANDLE) {}

VulkanPipeline::~VulkanPipeline() { Destroy(); }

void VulkanPipeline::Initialize(const GraphicsPipelineConfig &pipeline_config) {
  pipeline_config_ = pipeline_config;
  vk::Pipeline old_pipeline = vk_pipeline_;
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get logical device");

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

  std::vector<vk::PipelineShaderStageCreateInfo> shader_stages;// these store the configurations for vertex input, fragment, etc.

  // Vertex Input
  vk::PipelineVertexInputStateCreateInfo vertex_input_info = {};
  vertex_input_info.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
  vertex_input_info.flags = vk::PipelineVertexInputStateCreateFlags();
  vertex_input_info.pNext = nullptr;

  // Each vertex attribute has the following...
  // ------ Vertex Attribute Descriptions -----
  // - Binding
  // - Location
  // - Offset
  // - Format - you would still use the VK_FORMAT enums even if the attribute is e.g. position.
  //            vec2 position's format would be vk::Format::eR32G32Sfloat => 2 - 32-bit signed floats
  std::vector<vk::VertexInputAttributeDescription> attribute_descriptions = GetPosColorTexCoordAttributeDescriptions();
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
  vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

  // Bindings tell vulkan where position data, color data, etc. is in the buffer
  // Binding Descriptions have the following parameters...
  // 1. Binding No.
  // 2. Stride - # of bytes per vertex
  // 3. InputRate - tells if the data is per vertex or per instance
  vk::VertexInputBindingDescription binding_description = GetPosColorTexCoordBindingDescription();
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.pVertexBindingDescriptions = &binding_description;
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
  }
  GINFO("Successfully created vertex shader module");

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
  }
  GINFO("Successfully created fragment shader module");

  vk::PipelineShaderStageCreateInfo fragment_shader_stage = {};
  fragment_shader_stage.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
  fragment_shader_stage.stage = vk::ShaderStageFlagBits::eFragment;
  fragment_shader_stage.module = fragment_shader;
  fragment_shader_stage.pName = "main";
  shader_stages.push_back(fragment_shader_stage);

  // Adding shader modules to pipeline
  pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
  pipeline_create_info.pStages = shader_stages.data();

  // Depth Stencil - telling pipeline how to handle depth testing
  vk::PipelineDepthStencilStateCreateInfo depth_stencil_info = {};
  depth_stencil_info.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
  depth_stencil_info.flags = vk::PipelineDepthStencilStateCreateFlags();
  depth_stencil_info.depthTestEnable = VK_TRUE;
  depth_stencil_info.depthWriteEnable = VK_TRUE;
  depth_stencil_info.depthCompareOp = vk::CompareOp::eLess;
  depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_info.stencilTestEnable = VK_FALSE;
  pipeline_create_info.pDepthStencilState = &depth_stencil_info;

  // Multisampling
  vk::PipelineMultisampleStateCreateInfo multisampling_info = {};
  multisampling_info.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = vk::SampleCountFlagBits::e1;
  pipeline_create_info.pMultisampleState = &multisampling_info;

  // Color Blend - alpha transparency, etc.
  // This is doing no blending
  vk::PipelineColorBlendAttachmentState color_blend_attachment = {};
  color_blend_attachment.colorWriteMask = vk::ColorComponentFlags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG
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
  VK_ASSERT(render_pass != VK_NULL_HANDLE, "Failed to get Vulkan render pass");
  pipeline_create_info.renderPass = render_pass;

  // Extra stuff
  pipeline_create_info.basePipelineHandle = old_pipeline;// Optional - to base pipeline on

  // Create pipeline - for now just one pipeline
  VK_CHECK(device.createGraphicsPipelines(vk_pipeline_cache_, 1, &pipeline_create_info, nullptr, &vk_pipeline_),
           "Failed to create graphics pipeline");
  GINFO("Successfully created graphics pipeline");

  // Clean up shader modules and old pipeline
  if (old_pipeline != VK_NULL_HANDLE) {
    vkDestroyPipeline(device, old_pipeline, nullptr);
    old_pipeline = VK_NULL_HANDLE;
  }
  vkDestroyShaderModule(device, vertex_shader, nullptr);
  vkDestroyShaderModule(device, fragment_shader, nullptr);
}

void VulkanPipeline::Rebuild() {
  // use existing graphics pipeline config
  Initialize(pipeline_config_);
}

// This is so we can push constants and descriptor sets aka. Uniforms
void VulkanPipeline::CreatePipelineLayout() {
  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  vk::PipelineLayoutCreateInfo pipeline_layout_info = {};
  // here we are not setting ANY uniform data
  pipeline_layout_info.sType = vk::StructureType::ePipelineLayoutCreateInfo;
  pipeline_layout_info.setLayoutCount = 2;// this can push arbitrary data (usually large like an image) to pipeline
  const std::unordered_map<DescriptorPoolType, vk::DescriptorSetLayout> descriptor_set_layouts =
      context_.GetVulkanDescriptorPool().GetDescriptorSetLayouts();
  std::vector<vk::DescriptorSetLayout> set_layouts = {};
  set_layouts.reserve(descriptor_set_layouts.size());
  for (const std::pair<const DescriptorPoolType, vk::DescriptorSetLayout> &kLayout : descriptor_set_layouts) {
    set_layouts.push_back(kLayout.second);
  }
  pipeline_layout_info.pSetLayouts = set_layouts.data();

  //  pipeline_layout_info.pushConstantRangeCount = 0;  // this can only push small data to pipeline like one matrix
  //  pipeline_layout_info.pPushConstantRanges = nullptr;

  pipeline_layout_info.pushConstantRangeCount = 0;
  const vk::PushConstantRange push_constant_info = {};
  // pushing one matrix to pipeline as a "global" uniform
  //  push_constant_info.stageFlags = vk::ShaderStageFlagBits::eVertex;
  //  push_constant_info.offset = 0;
  //  push_constant_info.size = sizeof(glm::mat4);
  pipeline_layout_info.pPushConstantRanges = &push_constant_info;

  if (device.createPipelineLayout(&pipeline_layout_info, nullptr, &vk_pipeline_layout_) != vk::Result::eSuccess) {
    GERROR("Failed to create pipeline layout");
    vk_pipeline_layout_ = nullptr;
  } else {
    GINFO("Successfully created pipeline layout");
  }
}

void VulkanPipeline::Destroy() {
  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

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

vk::VertexInputBindingDescription VulkanPipeline::GetPosColorTexCoordBindingDescription() {
  // Provided by VK_VERSION_1_0
  //  typedef struct VkVertexInputBindingDescription {
  //    uint32_t             binding;
  //    uint32_t             stride;
  //    VkVertexInputRate    inputRate;
  //  } VkVertexInputBindingDescription;

  // Returns a binding description with POSITION and COLOR
  vk::VertexInputBindingDescription pos_color_binding = {};
  pos_color_binding.binding = 0;
  pos_color_binding.stride = sizeof(glm::vec2) + sizeof(glm::vec3) + sizeof(glm::vec2);// Position = vec3, Color = vec3, Texture Coords = vec2
  pos_color_binding.inputRate = vk::VertexInputRate::eVertex;
  return pos_color_binding;
}

std::vector<vk::VertexInputAttributeDescription> VulkanPipeline::GetPosColorTexCoordAttributeDescriptions() {
  // Provided by VK_VERSION_1_0
  //  typedef struct VkVertexInputAttributeDescription {
  //    uint32_t    location;   // corresponds in shader code as e.g. layout(location = 0)
  //    uint32_t    binding;
  //    VkFormat    format;
  //    uint32_t    offset;
  //  } VkVertexInputAttributeDescription;

  // Returns attribute descriptions for POSITION and COLOR
  std::vector<vk::VertexInputAttributeDescription> attribute_descriptions = {};
  // Position
  attribute_descriptions.emplace_back(0, 0, vk::Format::eR32G32Sfloat, 0);
  // Color
  // offset is +vec2 since position is of vec2 size
  attribute_descriptions.emplace_back(1, 0, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec2));
  // Texture Coords
  // offset is +vec3 since color is of vec3 size
  attribute_descriptions.emplace_back(2, 0, vk::Format::eR32G32Sfloat, sizeof(glm::vec2) + sizeof(glm::vec3));

  return attribute_descriptions;
}

}// namespace glaceon
