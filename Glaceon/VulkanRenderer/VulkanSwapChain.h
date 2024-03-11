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
  VkCommandBuffer commandBuffer;
};

class VulkanSwapChain {
 public:
  VulkanSwapChain(VulkanContext& context);
  void Initialize();
  void Destroy();

  VkSwapchainKHR GetSwapChain() { return swapChain; }
  std::vector<SwapChainFrame> &GetSwapChainFrames() { return swapChainFrames; }
  VkFormat GetSwapChainImageFormat() { return swapChainImageFormat; }
  VkExtent2D GetSwapChainExtent() { return swapChainExtent; }

  void RebuildSwapChain(int width, int height);

 private:
  VulkanContext& context;

  // relevant to swap chain
  VkSwapchainKHR swapChain;
  std::vector<SwapChainFrame> swapChainFrames;
  VkFormat swapChainImageFormat;
  VkExtent2D swapChainExtent;

  SwapChainSupportDetails swapChainSupport;
  VkFormat surfaceFormat;
  VkColorSpaceKHR colorSpace;
  VkPresentModeKHR presentMode;

  void PopulateSwapChainSupport();
  void CreateSwapChain();
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANRENDERER_VULKANSWAPCHAIN_H_
