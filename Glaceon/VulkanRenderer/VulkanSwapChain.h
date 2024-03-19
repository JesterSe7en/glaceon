#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

struct SwapChainSupportDetails {
  vk::SurfaceCapabilitiesKHR capabilities;
  std::vector<vk::SurfaceFormatKHR> formats;
  std::vector<vk::PresentModeKHR> present_modes;
};


struct SwapChainFrame {
  vk::Image image;
  vk::ImageView image_view;
  vk::Framebuffer frame_buffer;
};

class VulkanSwapChain {
 public:
  explicit VulkanSwapChain(VulkanContext& context);
  void Initialize();
  void Destroy();

  [[nodiscard]] const vk::SwapchainKHR &GetVkSwapchain() const {
    return vk_swapchain_;
  }
  std::vector<SwapChainFrame>& GetSwapChainFrames() { return swap_chain_frames_; }
  vk::Extent2D GetSwapChainExtent() { return swap_chain_extent_; }

  void RebuildSwapChain(int width, int height);

 private:
  VulkanContext& context_;

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
  void DestroyFrames();
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
