#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_

#include "../pch.h"
#include "VulkanUtils.h"

namespace glaceon {

class VulkanContext;

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;
};

// maybe put this in different file?
struct UniformBufferObject {
  glm::mat4 view;
  glm::mat4 proj;
  glm::mat4 view_proj;
};

struct SwapChainFrame {
  vk::Image image;
  vk::ImageView image_view;
  vk::Framebuffer frame_buffer;

  // drawing resources
  UniformBufferObject camera_data;
  VulkanUtils::Buffer camera_data_buffer;   // used to pass uniform data from CPU (aka camera_data) to GPU
  void *camera_data_mapped = nullptr;   // pointer to mapped memory
};


class VulkanSwapChain {
 public:
  explicit VulkanSwapChain(VulkanContext &context);
  void Initialize();
  void RebuildSwapChain(int width, int height);
  void Destroy();

  [[nodiscard]] const vk::SwapchainKHR &GetVkSwapchain() const { return vk_swapchain_; }
  std::vector<SwapChainFrame> &GetSwapChainFrames() { return swap_chain_frames_; }
  vk::Extent2D GetSwapChainExtent() { return swap_chain_extent_; }

 private:
  VulkanContext &context_;

  vk::SwapchainKHR vk_swapchain_;
  std::vector<SwapChainFrame> swap_chain_frames_;
  vk::Extent2D swap_chain_extent_;

  SwapChainSupportDetails swap_chain_support_;
  vk::Format surface_format_;
  vk::ColorSpaceKHR color_space_;
  vk::PresentModeKHR present_mode_;

  void PopulateSwapChainSupport();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateFrameBuffers();
  void CreateUboResources();
  void DestroyFrames();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
