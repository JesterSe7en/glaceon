#include "VulkanRenderPass.h"

#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"

namespace glaceon {

VulkanRenderPass::VulkanRenderPass(VulkanContext &context) : context_(context), vk_render_pass_(VK_NULL_HANDLE) {}

VulkanRenderPass::~VulkanRenderPass() { Destroy(); }

void VulkanRenderPass::Initialize(const VulkanRenderPassInput &input) {
  input_ = input;
  // An attachment is essentially an image or a memory buffer that serves as a rendering target or source within a
  // render pass Attachments are used to store the results of rendering operations, such as color, depth, or stencil
  // values. Each subpass in a render pass can read or write to the attachment

  // TODO: perhaps have a vector of attachments in the input?

  vk::AttachmentDescription color_attachment = {};
  color_attachment.flags = vk::AttachmentDescriptionFlags();
  // color_attachment.format = vk::Format::eB8G8R8A8Unorm;
  color_attachment.format = input.swapChainFormat;
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

  vk::AttachmentDescription depth_attachment = {};
  depth_attachment.format = input.depthFormat;
  depth_attachment.samples = vk::SampleCountFlagBits::e1;
  depth_attachment.loadOp = vk::AttachmentLoadOp::eClear;
  depth_attachment.storeOp = vk::AttachmentStoreOp::eDontCare;
  depth_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  depth_attachment.initialLayout = vk::ImageLayout::eUndefined;
  depth_attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  vk::AttachmentReference depth_attachment_ref = {};
  //  attachment is either an integer value identifying an attachment at the corresponding index in
  // VkRenderPassCreateInfo::pAttachments, or VK_ATTACHMENT_UNUSED to signify that this attachment is
  // not used.
  depth_attachment_ref.attachment = 1;
  depth_attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

  // A render pass always has at LEAST one subpass
  vk::SubpassDescription subpass_description = {};
  subpass_description.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_ref;
  subpass_description.pDepthStencilAttachment = &depth_attachment_ref;// attach depth buffer reference to subpass

  std::vector<vk::AttachmentDescription> attachments;
  attachments.push_back(color_attachment);
  attachments.push_back(depth_attachment);

  vk::RenderPassCreateInfo render_pass_create_info = {};
  render_pass_create_info.sType = vk::StructureType::eRenderPassCreateInfo;
  render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
  render_pass_create_info.pAttachments = attachments.data();
  render_pass_create_info.subpassCount = 1;
  render_pass_create_info.pSubpasses = &subpass_description;

  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  if (device.createRenderPass(&render_pass_create_info, nullptr, &vk_render_pass_) != vk::Result::eSuccess) {
    GERROR("Failed to create render pass");
  } else {
    GINFO("Successfully created render pass");
  }
}

void VulkanRenderPass::Rebuild() {
  Destroy();
  Initialize(input_);
}

void VulkanRenderPass::Destroy() {
  if (vk_render_pass_ != nullptr) {
    const vk::Device device = context_.GetVulkanLogicalDevice();
    VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

    device.destroy(vk_render_pass_, nullptr);
    vk_render_pass_ = VK_NULL_HANDLE;
  }
}

}// namespace glaceon
