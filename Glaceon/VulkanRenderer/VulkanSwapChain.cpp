//
// Created by alyxc on 3/7/2024.
//

#include "VulkanSwapChain.h"

#include <vulkan/vk_enum_string_helper.h>

#include <vulkan/vulkan.hpp>

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {
VulkanSwapChain::VulkanSwapChain(VulkanContext& context) : context(context) {}

void VulkanSwapChain::Initialize() {
  // probably want to request from user a specific format, color space, and present mode
  // then use that to create the swap chain
  // for now, hardcode it
  // FIFO present mode is guaranteed to be supported
  surfaceFormat = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
  colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
  presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;

  PopulateSwapChainSupport();

  CreateSwapChain();
}

void VulkanSwapChain::PopulateSwapChainSupport() {
  auto physicalDevice = context.GetVulkanPhysicalDevice();
  auto surface = context.GetSurface();

  VkResult res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapChainSupport.capabilities);

  if (res != VK_SUCCESS) {
    GERROR("Failed to get swap chain capabilities");
    return;
  }

  //  typedef struct VkSurfaceCapabilitiesKHR {
  //    uint32_t                         minImageCount;
  //    uint32_t                         maxImageCount;
  //    VkExtent2D                       currentExtent; // basically the current size of the images in the swap chain
  //    VkExtent2D                       minImageExtent;
  //    VkExtent2D                       maxImageExtent;
  //    uint32_t                         maxImageArrayLayers;

  //    VkSurfaceTransformFlagsKHR       supportedTransforms;
  // bitfield of VkSurfaceTransformFlagBitsKHR:
  // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceTransformFlagBitsKHR.html

  //    VkSurfaceTransformFlagBitsKHR    currentTransform;

  //    VkCompositeAlphaFlagsKHR         supportedCompositeAlpha;
  // bitfield of VkCompositeAlphaFlagBitsKHR:
  // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCompositeAlphaFlagBitsKHR.html

  //    VkImageUsageFlags                supportedUsageFlags;
  // bitfield of VkImageUsageFlagBits:
  // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkImageUsageFlagBits.html

  //  } VkSurfaceCapabilitiesKHR;

#if _DEBUG
  GTRACE("Swap chain capabilities:");
  GTRACE("  minImageCount: {}", swapChainSupport.capabilities.minImageCount);
  GTRACE("  maxImageCount: {}", swapChainSupport.capabilities.maxImageCount);
#endif

  uint32_t format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &format_count, nullptr);
  if (format_count != 0) {
    swapChainSupport.formats.resize(format_count);
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceFormatKHR.html
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &format_count, swapChainSupport.formats.data());
  }

#if _DEBUG
  for (auto& format : swapChainSupport.formats) {
    // print out supported surface formats
    GTRACE("Supported surface format:");
    GTRACE("  format: {}", string_VkFormat(format.format));
    GTRACE("  colorSpace: {}", string_VkColorSpaceKHR(format.colorSpace));
  }
#endif

  // check if surfaceFormat and colorSpace is supported
  bool found = false;
  for (auto& format : swapChainSupport.formats) {
    if (format.format == surfaceFormat && format.colorSpace == colorSpace) {
      found = true;
      GTRACE("Device supports targeted surface format & color space");
      GTRACE("Target Surface format: {}", string_VkFormat(surfaceFormat));
      GTRACE("Target Color space: {}", string_VkColorSpaceKHR(colorSpace));
      break;
    }
  }
  if (!found) {
    GERROR("Surface format not found");
    return;
  }

  vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &format_count, nullptr);
  if (format_count != 0) {
    swapChainSupport.presentModes.resize(format_count);
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &format_count,
                                              swapChainSupport.presentModes.data());
  }

#if _DEBUG
  for (auto& mode : swapChainSupport.presentModes) {
    // print out supported present modes
    GTRACE("Supported present mode:");
    GTRACE("  mode: {}", string_VkPresentModeKHR(mode));
  }
#endif

  // check if presentMode is supported
  found = false;
  for (auto& mode : swapChainSupport.presentModes) {
    if (mode == presentMode) {
      found = true;
      GTRACE("Device supports targeted present mode");
      GTRACE("Target Present mode: {}", string_VkPresentModeKHR(presentMode));
      break;
    }
  }
  if (!found) {
    GERROR("Requested Present mode not found, defaulting to VK_PRESENT_MODE_FIFO_KHR");
    presentMode = VK_PRESENT_MODE_FIFO_KHR;
    return;
  }
}
void VulkanSwapChain::CreateSwapChain() {
  auto surface = context.GetSurface();

  uint32_t imageCount =
      std::min(swapChainSupport.capabilities.maxImageCount, swapChainSupport.capabilities.minImageCount + 1);

  //  typedef struct VkSwapchainCreateInfoKHR {
  //    VkStructureType                  sType;
  //    const void*                      pNext;
  //    VkSwapchainCreateFlagsKHR        flags;
  //    VkSurfaceKHR                     surface;
  //    uint32_t                         minImageCount;
  //    VkFormat                         imageFormat;
  //    VkColorSpaceKHR                  imageColorSpace;
  //    VkExtent2D                       imageExtent;
  //    uint32_t                         imageArrayLayers;
  //    VkImageUsageFlags                imageUsage;
  //    VkSharingMode                    imageSharingMode;
  //    uint32_t                         queueFamilyIndexCount;
  //    const uint32_t*                  pQueueFamilyIndices;
  //    VkSurfaceTransformFlagBitsKHR    preTransform;
  //    VkCompositeAlphaFlagBitsKHR      compositeAlpha;
  //    VkPresentModeKHR                 presentMode;
  //    VkBool32                         clipped;
  //    VkSwapchainKHR                   oldSwapchain;
  //  } VkSwapchainCreateInfoKHR;

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = surface;
  createInfo.minImageCount = imageCount;
  createInfo.imageFormat = surfaceFormat;
  createInfo.imageColorSpace = colorSpace;
  createInfo.imageExtent = swapChainSupport.capabilities.currentExtent;
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  assert(context.GetQueueIndexes().graphicsFamily.has_value() && context.GetQueueIndexes().presentFamily.has_value());

  QueueIndexes indexes = context.GetQueueIndexes();
  uint32_t queueFamilyIndices[] = {indexes.graphicsFamily.value(), indexes.presentFamily.value()};
  // since there is a possibility that the graphics queue and the present queue are not the same index,
  // there will be two separate queue families acting on the swap chain.  We need to let the swap chain
  // know which queues will be used.
  // If they ARE THE SAME index, we can set imageSharingMode to VK_SHARING_MODE_EXCLUSIVE
  if (queueFamilyIndices[0] != queueFamilyIndices[1]) {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  } else {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = nullptr;
  }

  createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
  createInfo.presentMode = presentMode;
  createInfo.clipped = VK_TRUE;  // if a window is rendered above the rendering window, it will be clipped
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.oldSwapchain = nullptr;  // used during re-initialization from old to speed up creation

  VkResult res = vkCreateSwapchainKHR(context.GetVulkanLogicalDevice(), &createInfo, nullptr, &swapChain);

  if (res != VK_SUCCESS) {
    GERROR("Failed to create swap chain");
  } else {
    GTRACE("Successfully created swap chain");
  }
}
}  // namespace Glaceon