#include "VulkanSync.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {
VulkanSync::VulkanSync(VulkanContext& context) : context(context) {}
VulkanSync::~VulkanSync() {
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);
  for (auto semaphore : imageAvailableSemaphores) {
    if (semaphore != VK_NULL_HANDLE) {
      vkDestroySemaphore(device, semaphore, nullptr);
    }
  }
  for (auto semaphore : renderFinishedSemaphores) {
    if (semaphore != VK_NULL_HANDLE) {
      vkDestroySemaphore(device, semaphore, nullptr);
    }
  }
  if (inFlightFence != VK_NULL_HANDLE) {
    vkDestroyFence(device, inFlightFence, nullptr);
  }
}
void VulkanSync::Initialize() {
  VkDevice device = context.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);

  assert(context.GetVulkanSwapChain().GetVkSwapChain() != VK_NULL_HANDLE &&
         context.GetVulkanSwapChain().GetSwapChainFrames().size() != 0);

  size_t maxFramesInFlight = context.GetVulkanSwapChain().GetSwapChainFrames().size();

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphoreInfo.pNext = nullptr;
  semaphoreInfo.flags = 0;
  for (uint32_t i = 0; i < maxFramesInFlight; i++) {
    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
      GERROR("Failed to create image available semaphore")
      return;
    } else {
      imageAvailableSemaphores.push_back(semaphore);
    }
  }
  GINFO("Successfully created image available semaphore")

  for (uint32_t i = 0; i < maxFramesInFlight; i++) {
    VkSemaphore semaphore;
    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
      GERROR("Failed to create render finished semaphore")
      return;
    } else {
      renderFinishedSemaphores.push_back(semaphore);
    }
  }
  GINFO("Successfully created render finished semaphore")

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
