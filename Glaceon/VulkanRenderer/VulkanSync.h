#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_

namespace Glaceon {

// https://vulkan-tutorial.com/Drawing_a_triangle/Drawing/Frames_in_flight
// The point of this class is to manage the synchronization objects between the GPU and the CPU.
// Fences are they are used to ensure that the GPU has finished all work through an entire pipeline; ensure draw command
// are complete and not trying to run again before previous commands are completed.
//
// Semaphores are used to flag when an image is ready to be used by the GPU or an image is ready to be presented aka.
// imageAvailableSemaphore and renderFinishedSemaphore

class VulkanContext;

class VulkanSync {
 public:
  VulkanSync(VulkanContext &context);

  void Initialize();
  void Destroy();

  const std::vector<VkSemaphore> &GetImageAvailableSemaphores() const { return imageAvailableSemaphores; }
  const std::vector<VkSemaphore> &GetRenderFinishedSemaphores() const { return renderFinishedSemaphores; }
  const VkFence &GetInFlightFence() const { return inFlightFence; }

 private:
  VulkanContext &context;

  std::vector<VkSemaphore> imageAvailableSemaphores;
  std::vector<VkSemaphore> renderFinishedSemaphores;

  VkFence inFlightFence;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_