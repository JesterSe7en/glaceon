#include "VulkanSync.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace glaceon {

VulkanSync::VulkanSync(VulkanContext &context) : context_(context) {}

void VulkanSync::Initialize() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);

  assert(context_.GetVulkanSwapChain().GetVkSwapchain() != VK_NULL_HANDLE &&
      !context_.GetVulkanSwapChain().GetSwapChainFrames().empty());

  size_t max_frames_in_flight = context_.GetVulkanSwapChain().GetSwapChainFrames().size();

  vk::SemaphoreCreateInfo semaphore_create_info = {};
  semaphore_create_info.sType = vk::StructureType::eSemaphoreCreateInfo;
  semaphore_create_info.pNext = nullptr;
  semaphore_create_info.flags = vk::SemaphoreCreateFlags();
  for (uint32_t i = 0; i < max_frames_in_flight; i++) {
    vk::Semaphore semaphore;
    if (device.createSemaphore(&semaphore_create_info, nullptr, &semaphore) != vk::Result::eSuccess) {
      GERROR("Failed to create image available semaphore");
      return;
    } else {
      image_available_semaphores_.push_back(semaphore);
    }
  }
  GINFO("Successfully created image available semaphore");

  for (uint32_t i = 0; i < max_frames_in_flight; i++) {
    vk::Semaphore semaphore;
    if (device.createSemaphore(&semaphore_create_info, nullptr, &semaphore) != vk::Result::eSuccess) {
      GERROR("Failed to create image available semaphore");
      return;
    } else {
      render_finished_semaphores_.push_back(semaphore);
    }
  }
  GINFO("Successfully created render finished semaphore");

  vk::FenceCreateInfo fence_create_info = {};
  fence_create_info.sType = vk::StructureType::eFenceCreateInfo;
  fence_create_info.pNext = nullptr;
  fence_create_info.flags =
      vk::FenceCreateFlags(vk::FenceCreateFlagBits::eSignaled);  // initialize fence in signaled state (ready to be used)
  for (uint32_t i = 0; i < max_frames_in_flight; i++) {
    vk::Fence fence;
    if (device.createFence(&fence_create_info, nullptr, &fence) != vk::Result::eSuccess) {
      GERROR("Failed to create in flight fence");
      return;
    } else {
      in_flight_fences_.push_back(fence);
    }
  }
}

void VulkanSync::Destroy() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  assert(device != VK_NULL_HANDLE);

  for (vk::Semaphore semaphore : image_available_semaphores_) {
    if (semaphore != VK_NULL_HANDLE) {
      device.destroy(semaphore, nullptr);
      semaphore = VK_NULL_HANDLE;
    }
  }
  image_available_semaphores_.clear();
  for (vk::Semaphore semaphore : render_finished_semaphores_) {
    if (semaphore != VK_NULL_HANDLE) {
      device.destroy(semaphore, nullptr);
      semaphore = VK_NULL_HANDLE;
    }
  }
  render_finished_semaphores_.clear();
  for (vk::Fence fence : in_flight_fences_) {
    if (fence != VK_NULL_HANDLE) {
      device.destroy(fence, nullptr);
      fence = VK_NULL_HANDLE;
    }
  }
  in_flight_fences_.clear();
}

}  // namespace Glaceon
