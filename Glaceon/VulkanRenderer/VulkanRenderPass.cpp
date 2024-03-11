//
// Created by alyxc on 3/10/2024.
//

#include "VulkanRenderPass.h"

#include "VulkanContext.h"
#include "../Logger.h"

namespace Glaceon {

VulkanRenderPass::VulkanRenderPass(VulkanContext& context) : context(context) {}

void VulkanRenderPass::Initialize() {

  VkDevice device = context.GetVulkanDevice().GetLogicalDevice();


  VkAttachmentDescription attachment = {};
  attachment.format = context.GetVulkanSwapChain().GetSwapChainImageFormat();
  attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  attachment.loadOp =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  VkAttachmentReference color_attachment = {};
  color_attachment.attachment = 0;
  color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment;
  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  VkRenderPassCreateInfo info = {};
  info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  info.attachmentCount = 1;
  info.pAttachments = &attachment;
  info.subpassCount = 1;
  info.pSubpasses = &subpass;
  info.dependencyCount = 1;
  info.pDependencies = &dependency;
  VkResult err = vkCreateRenderPass(device, &info, nullptr, &renderPass);
  if (err != VK_SUCCESS) {
    GERROR("Failed to create render pass");
  } else {
    GINFO("Successfully created render pass");
  }
}
}  // namespace Glaceon