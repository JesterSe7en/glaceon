#include "VulkanSync.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {
VulkanSync::VulkanSync(VulkanContext& context) : context(context) {}
VulkanSync::~VulkanSync() {
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);
  if (imageAvailableSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
  }
  if (renderFinishedSemaphore != VK_NULL_HANDLE) {
    vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
  }
  if (inFlightFence != VK_NULL_HANDLE) {
    vkDestroyFence(device, inFlightFence, nullptr);
  }
}
void VulkanSync::Initialize() {
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;
  if (vkCreateSemaphore(context.GetVulkanLogicalDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore) !=
      VK_SUCCESS) {
    GERROR("Failed to create image available semaphore")
  } else {
    GINFO("Successfully created image available semaphore")
  }
  if (vkCreateSemaphore(context.GetVulkanLogicalDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore) !=
      VK_SUCCESS) {
    GERROR("Failed to create render finished semaphore")
  } else {
    GINFO("Successfully created render finished semaphore")
  }

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.pNext = nullptr;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // initialize fence in signaled state (ready to be used)
  if (vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
    GERROR("Failed to create in flight fence")
  } else {
    GINFO("Successfully created in flight fence")
  }
}
}  // namespace Glaceon