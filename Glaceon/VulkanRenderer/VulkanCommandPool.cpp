#include "VulkanCommandPool.h"

#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanCommandPool::VulkanCommandPool(VulkanContext &context) : context_(context) {}

VulkanCommandPool::~VulkanCommandPool() { Destroy(); }

void VulkanCommandPool::Initialize() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");
  VK_ASSERT(context_.GetQueueIndexes().graphics_family.has_value(), "Failed to get graphics queue family index");

  // create command pool
  vk::CommandPoolCreateInfo command_pool_create_info = {};
  command_pool_create_info.sType = vk::StructureType::eCommandPoolCreateInfo;
  command_pool_create_info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
  command_pool_create_info.queueFamilyIndex = context_.GetQueueIndexes().graphics_family.value();
  if (device.createCommandPool(&command_pool_create_info, nullptr, &vk_command_pool_) != vk::Result::eSuccess) {
    GERROR("Failed to create command pool");
    return;
  } else {
    GINFO("Successfully created command pool");
  }

  // initialize main command buffer
  vk::CommandBufferAllocateInfo allocate_info = {};
  allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  allocate_info.level = vk::CommandBufferLevel::ePrimary;
  allocate_info.commandPool = vk_command_pool_;
  allocate_info.commandBufferCount = 1;
  if (device.allocateCommandBuffers(&allocate_info, &vk_main_command_buffer_) != vk::Result::eSuccess) {
    GERROR("Failed to allocate main command buffer");
    return;
  } else {
    GINFO("Successfully allocated main command buffer");
  }

  // create command buffer for each swap chain frame
  std::vector<SwapChainFrame> swap_chain_frames = context_.GetVulkanSwapChain().GetSwapChainFrames();
  vk_frame_command_buffers_.resize(swap_chain_frames.size());
  // Allocate command buffers outside the loop
  for (size_t i = 0; i < swap_chain_frames.size(); ++i) {
    if (device.allocateCommandBuffers(&allocate_info, &vk_frame_command_buffers_[i]) != vk::Result::eSuccess) {
      GERROR("Failed to allocate command buffer for swap chain frame");
      return;
    }
  }

  GINFO("Successfully allocated command buffers for swap chain frames");
}

void VulkanCommandPool::Destroy() {
  if (vk_command_pool_ != VK_NULL_HANDLE) {
    vk::Device device = context_.GetVulkanLogicalDevice();
    VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

    device.destroy(vk_command_pool_);
    vk_command_pool_ = VK_NULL_HANDLE;
  }
}
void VulkanCommandPool::ResetCommandPool() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  device.resetCommandPool(vk_command_pool_);
}

void VulkanCommandPool::RebuildCommandBuffers() {
  ResetCommandPool();
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  // destroy all old command buffers
  device.freeCommandBuffers(vk_command_pool_, 1, &vk_main_command_buffer_);
  device.freeCommandBuffers(vk_command_pool_, static_cast<uint32_t>(vk_frame_command_buffers_.size()), vk_frame_command_buffers_.data());

  vk_frame_command_buffers_.clear();
  std::vector<SwapChainFrame> swap_chain_frames = context_.GetVulkanSwapChain().GetSwapChainFrames();
  vk_frame_command_buffers_.resize(swap_chain_frames.size());

  vk::CommandBufferAllocateInfo command_buffer_allocate_info = {};
  command_buffer_allocate_info.sType = vk::StructureType::eCommandBufferAllocateInfo;
  command_buffer_allocate_info.level = vk::CommandBufferLevel::ePrimary;
  command_buffer_allocate_info.commandPool = vk_command_pool_;
  command_buffer_allocate_info.commandBufferCount = 1;

  if (device.allocateCommandBuffers(&command_buffer_allocate_info, &vk_main_command_buffer_) != vk::Result::eSuccess) {
    GERROR("Failed to allocate main command buffer");
    return;
  }

  for (size_t i = 0; i < swap_chain_frames.size(); ++i) {
    // allocate command buffers for each swap chain frame
    if (device.allocateCommandBuffers(&command_buffer_allocate_info, &vk_frame_command_buffers_[i]) != vk::Result::eSuccess) {
      GERROR("Failed to allocate command buffer for swap chain frame");
      return;
    }
  }
  GINFO("Successfully rebuilt command buffers");
}
}// namespace glaceon
