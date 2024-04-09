#include "VulkanRenderPass.h"

#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"


namespace glaceon {

VulkanRenderPass::VulkanRenderPass(VulkanContext &context) : context_(context), vk_render_pass_(VK_NULL_HANDLE) {}

void VulkanRenderPass::Initialize() {
  // An attachment is essentially an image or a memory buffer that serves as a rendering target or source within a
  // render pass Attachments are used to store the results of rendering operations, such as color, depth, or stencil
  // values. Each subpass in a render pass can read or write to the attachment

  vk::AttachmentDescription color_attachment = {};
  color_attachment.format = vk::Format::eB8G8R8A8Unorm;
  // TODO: Uncomment this out once we have a swap chain
  //    colorAttachment.format = context.GetVulkanSwapChain().GetSwapChainImageFormat();
  color_attachment.samples = vk::SampleCountFlagBits::e1;
  color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
  color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
  color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  color_attachment.initialLayout = vk::ImageLayout::eUndefined;
  color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference color_attachment_ref = {};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

  // A render pass always has at LEAST one subpass
  vk::SubpassDescription subpass_description = {};
  subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_ref;

  vk::RenderPassCreateInfo render_pass_create_info = {};
  render_pass_create_info.sType = vk::StructureType::eRenderPassCreateInfo;
  render_pass_create_info.attachmentCount = 1;
  render_pass_create_info.pAttachments = &color_attachment;
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &subpass_description;

  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  if (device.createRenderPass(&render_pass_create_info, nullptr, &vk_render_pass_) != vk::Result::eSuccess) {
    GERROR("Failed to create render pass");
  } else {
    GINFO("Successfully created render pass");
  }
}

void VulkanRenderPass::Rebuild() {
  Destroy();
  Initialize();
}

void VulkanRenderPass::Destroy() {
  if (vk_render_pass_ != nullptr) {
    vk::Device device = context_.GetVulkanLogicalDevice();
    VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

    device.destroy(vk_render_pass_, nullptr);
    vk_render_pass_ = VK_NULL_HANDLE;
  }
}

}// namespace glaceon
