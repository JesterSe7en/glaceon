#ifndef GLACEON_VULKANSWAPCHAIN_H_
#define GLACEON_VULKANSWAPCHAIN_H_

#include <vulkan/vulkan.h>

#include <vector>

namespace Glaceon {

class VulkanContext;

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

class VulkanSwapChain {
 public:
  VulkanSwapChain(VulkanContext& context);
  void Initialize();
  // TODO: Add a function to recreate the swap chain using old swap chain
  void Destroy();

 private:
  VulkanContext& context;

  // relevant to swap chain
  VkSwapchainKHR swapChain;
  std::vector<VkImage> swapChainImages;
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
