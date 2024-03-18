#ifndef GLACEON_VULKANSWAPCHAIN_H_
#define GLACEON_VULKANSWAPCHAIN_H_

#include "../pch.h"

namespace Glaceon {

class VulkanContext;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

struct SwapChainFrame {
  VkImage image;
  VkImageView imageView;
  // for rendering
  VkFramebuffer framebuffer;
};

class VulkanSwapChain {
 public:
  VulkanSwapChain(VulkanContext& context);
  void Initialize();
  void Destroy();

  VkSwapchainKHR GetVkSwapChain() { return swapChain; }
  std::vector<SwapChainFrame>& GetSwapChainFrames() { return swapChainFrames; }
  VkExtent2D GetSwapChainExtent() { return swapChainExtent; }

  void RebuildSwapChain(int width, int height);

 private:
  VulkanContext& context;

  // relevant to swap chain
  VkSwapchainKHR swapChain;
  std::vector<SwapChainFrame> swapChainFrames;
  std::vector<VkFramebuffer> swapChainFrameBuffers;

 private:
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  SwapChainSupportDetails swapChainSupport;
  VkFormat surfaceFormat;
  VkColorSpaceKHR colorSpace;
  VkPresentModeKHR presentMode;

  void PopulateSwapChainSupport();
  void CreateSwapChain();
  void CreateImageViews();
  void CreateFrameBuffers();
  void DestroyFrames();
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
