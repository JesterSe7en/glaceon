#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_

namespace glaceon {

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
  explicit VulkanSync(VulkanContext &context);

  void Initialize();
  void Destroy();

  [[nodiscard]] const std::vector<vk::Semaphore> &GetImageAvailableSemaphores() const { return image_available_semaphores_; }
  [[nodiscard]] const std::vector<vk::Semaphore> &GetRenderFinishedSemaphores() const { return render_finished_semaphores_; }
  [[nodiscard]] const vk::Fence &GetInFlightFence() const { return in_flight_fence_; }

 private:
  VulkanContext &context_;

  std::vector<vk::Semaphore> image_available_semaphores_;
  std::vector<vk::Semaphore> render_finished_semaphores_;
  vk::Fence in_flight_fence_;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANSYNC_H_
