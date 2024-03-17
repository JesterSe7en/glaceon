#include "VulkanCommandPool.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {
VulkanCommandPool::VulkanCommandPool(VulkanContext& context) : context(context) {}
void VulkanCommandPool::Initialize() {
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);
  assert(context.GetQueueIndexes().graphicsFamily.has_value());

  VkCommandPoolCreateInfo poolInfo = {};
  poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  poolInfo.queueFamilyIndex = context.GetQueueIndexes().graphicsFamily.value();
  if (vkCreateCommandPool(device, &poolInfo, nullptr, &vk_command_pool_) != VK_SUCCESS) {
    GERROR("Failed to create command pool")
    return;
  } else {
    GINFO("Successfully created command pool")
  }

  // initialize main command buffer
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandPool = vk_command_pool_;
  allocInfo.commandBufferCount = 1;
  if (vkAllocateCommandBuffers(device, &allocInfo, &vk_main_command_buffer_) != VK_SUCCESS) {
    GERROR("Failed to allocate main command buffer")
    return;
  } else {
    GINFO("Successfully allocated main command buffer")
  }

  // create command buffer for each swap chain frame
  std::vector<SwapChainFrame> swapChainFrames = context.GetVulkanSwapChain().GetSwapChainFrames();
  for (auto& swapChainFrame : swapChainFrames) {
    if (vkAllocateCommandBuffers(device, &allocInfo, &swapChainFrame.commandBuffer) != VK_SUCCESS) {
      GERROR("Failed to allocate command buffer for swap chain frame")
      return;
    }
  }
  GINFO("Successfully allocated command buffers for swap chain frames")
}

void VulkanCommandPool::Destroy() {
  if (vk_command_pool_ != VK_NULL_HANDLE) {
    vkDestroyCommandPool(context.GetVulkanLogicalDevice(), vk_command_pool_, nullptr);
    vk_command_pool_ = VK_NULL_HANDLE;
  }
}
}  // namespace Glaceon