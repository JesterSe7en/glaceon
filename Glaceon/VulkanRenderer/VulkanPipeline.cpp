#include "VulkanPipeline.h"

#include <vulkan/vulkan_core.h>

#include "../Logger.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"

namespace Glaceon {

VulkanPipeline::VulkanPipeline(VulkanContext& context) : context(context) {}

void VulkanPipeline::Initialize(GraphicsPipelineConfig pipelineConfig) {

  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != nullptr);

  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.pNext = nullptr;
  pipelineInfo.flags = 0;

  std::vector<VkPipelineShaderStageCreateInfo >
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
  pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

  // Vertex Shader
  if (pipelineConfig.vertexShaderFile.empty()) {
    GERROR("No vertex shader specified");
    return;
  }

  VkShaderModule vertexShader = VulkanUtils::CreateShaderModule(device, pipelineConfig.vertexShaderFile);
  if (vertexShader == nullptr) {
    GERROR("Failed to create vertex shader module");
    return;
  } else {
    GTRACE("Created vertex shader module");
  }

  VkPipelineShaderStageCreateInfo vertexShaderStage = {};
  vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStage.module = vertexShader;
  vertexShaderStage.pName = "main";
  shaderStages.push_back(vertexShaderStage);

  // Viewport and Scissor
  VkExtent2D extent = context.GetVulkanSwapChain().GetSwapChainExtent();
  VkViewport viewport = {};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(extent.width);
  viewport.height = static_cast<float>(extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  VkRect2D scissor = {};
  scissor.offset = {0, 0};
  scissor.extent = extent;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = &viewport;
  viewportState.scissorCount = 1;
  viewportState.pScissors = &scissor;
  pipelineInfo.pViewportState = &viewportState;

  // Rasteurizer - fills in all the fragments aka pixels
  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.pNext = nullptr;
  rasterizer.flags = 0;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  pipelineInfo.pRasterizationState = &rasterizer;

  // Fragment shader
  if (pipelineConfig.vertexShaderFile.empty()) {
    GERROR("No fragment shader specified");
    return;
  }

  VkShaderModule fragmentShader = VulkanUtils::CreateShaderModule(device, pipelineConfig.fragmentShaderFile);
  if (fragmentShader == nullptr) {
    GERROR("Failed to create fragment shader module");
    return;
  } else {
    GTRACE("Created fragment shader module");
  }

  VkPipelineShaderStageCreateInfo fragmentShaderStage = {};
  fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStage.module = fragmentShader;
  fragmentShaderStage.pName = "main";
  shaderStages.push_back(fragmentShaderStage);

  // Adding shader modules to pipeline
  pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
  pipelineInfo.pStages = shaderStages.data();

  // Multisampling
  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  pipelineInfo.pMultisampleState = &multisampling;

  // Color Blend - alpha transparency, etc.
  // This is doing no blending
  VkPipelineColorBlendAttachmentState colorBlendingAttachment = {};
  colorBlendingAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendingAttachment.blendEnable = VK_FALSE;
  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendingAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;
  pipelineInfo.pColorBlendState = &colorBlending;

  // Pipeline layout
  CreatePipelineLayout();
  pipelineInfo.layout = pipelineLayout;

  // Render Pass
  CreateRenderPass();
  pipelineInfo.renderPass = renderPass;

  // Extra stuff
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional - to base pipeline on

  // Create pipeline - for now just one pipeline
  if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
    GERROR("Failed to create graphics pipeline");
    pipeline = nullptr;
  } else {
    GTRACE("Created graphics pipeline");
  }

  // Clean up shader modules
  vkDestroyShaderModule(device, vertexShader, nullptr);
  vkDestroyShaderModule(device, fragmentShader, nullptr);

  // TODO: Probably need access to these from the pipeline
  // 1. Pipeline layout
  // 2. Render pass
  // 3. The pipeline itself
}

// This is so we can push constants and descriptor sets aka. Uniforms
void VulkanPipeline::CreatePipelineLayout() {
  GTRACE("Creating pipeline layout...");
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != nullptr);

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  // here we are not setting ANY uniform data
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;  // this can push arbitrary data (usually large like an image) to pipeline
  pipelineLayoutInfo.pSetLayouts = nullptr;
  pipelineLayoutInfo.pushConstantRangeCount = 0;  // this can only push small data to pipeline like one matrix
  pipelineLayoutInfo.pPushConstantRanges = nullptr;
  if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
    GERROR("Failed to create pipeline layout");
    pipelineLayout = nullptr;
  } else {
    GTRACE("Created pipeline layout");
  }
}

void VulkanPipeline::CreateRenderPass() {
  // TODO: Probably use the VulkanRenderPass class instead, right now it does not accept any config

  // An attachment is essentially an image or a memory buffer that serves as a rendering target or source within a render pass
  // Attachments are used to store the results of rendering operations, such as color, depth, or stencil values.
  // Each subpass in a render pass can read or write to the attachment
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = context.GetVulkanSwapChain().GetSwapChainImageFormat();
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // A render pass always has at LEAST one subpass
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pAttachments = &colorAttachment;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;

  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != nullptr);

  if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
    GERROR("Failed to create render pass");
    renderPass = nullptr;
  } else {
    GTRACE("Created render pass");
  }
}

}  // namespace Glaceon
