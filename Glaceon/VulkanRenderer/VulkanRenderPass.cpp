#include "VulkanRenderPass.h"

#include "VulkanContext.h"
#include "../Logger.h"

namespace Glaceon {

VulkanRenderPass::VulkanRenderPass(VulkanContext& context) : context(context) {}

void VulkanRenderPass::Initialize() {
  // An attachment is essentially an image or a memory buffer that serves as a rendering target or source within a
  // render pass Attachments are used to store the results of rendering operations, such as color, depth, or stencil
  // values. Each subpass in a render pass can read or write to the attachment
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = VK_FORMAT_B8G8R8A8_UNORM;
  // TODO: Uncomment this out once we have a swap chain
  //  colorAttachment.format = context.GetVulkanSwapChain().GetSwapChainImageFormat();
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
    GINFO("Successfully created render pass");
  }
}
const VkRenderPass& VulkanRenderPass::GetVkRenderPass() const { return renderPass; }
}  // namespace Glaceon