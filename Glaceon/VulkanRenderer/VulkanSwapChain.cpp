#include "VulkanSwapChain.h"

#include "../Core/Base.h"
#include "../Core/Logger.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanSwapChain::VulkanSwapChain(VulkanContext &context)
    : context_(context),
      vk_swapchain_(VK_NULL_HANDLE),
      // probably want to request from user a specific format, color space, and present mode
      // then use that to create the swap chain
      // for now, hardcode it
      // FIFO present mode is guaranteed to be supported
      surface_format_(vk::Format::eB8G8R8A8Unorm),
      color_space_(vk::ColorSpaceKHR::eSrgbNonlinear),
      present_mode_(vk::PresentModeKHR::eMailbox) {}

VulkanSwapChain::~VulkanSwapChain() { Destroy(); }

// create swap chain, get images, create image views, then create the frame buffers
void VulkanSwapChain::Initialize() {
  PopulateSwapChainSupport();
  CreateSwapChain();
  CreateImageViews();
  CreateFrameBuffers();
  CreateDescriptorResources();
}

void VulkanSwapChain::PopulateSwapChainSupport() {
  const vk::PhysicalDevice physical_device = context_.GetVulkanPhysicalDevice();
  const vk::SurfaceKHR surface = context_.GetSurface();

  if (physical_device.getSurfaceCapabilitiesKHR(surface, &swap_chain_support_.capabilities) != vk::Result::eSuccess) {
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
  GTRACE("  minImageCount: {}", swap_chain_support_.capabilities.minImageCount);
  GTRACE("  maxImageCount: {}", swap_chain_support_.capabilities.maxImageCount);
  GTRACE("  minExtent: width = {}  height = {}", swap_chain_support_.capabilities.minImageExtent.width,
         swap_chain_support_.capabilities.minImageExtent.height);
  GTRACE("  maxExtent: width = {}  height = {}", swap_chain_support_.capabilities.maxImageExtent.width,
         swap_chain_support_.capabilities.maxImageExtent.height);
  GTRACE("  currentExtent: width = {}  height = {}", swap_chain_support_.capabilities.currentExtent.width,
         swap_chain_support_.capabilities.currentExtent.height);
#endif

  uint32_t format_count = 0;
  (void) physical_device.getSurfaceFormatsKHR(surface, &format_count, nullptr);
  if (format_count != 0) {
    swap_chain_support_.formats.resize(format_count);
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkSurfaceFormatKHR.html
    if (physical_device.getSurfaceFormatsKHR(surface, &format_count, swap_chain_support_.formats.data()) != vk::Result::eSuccess) {
      GERROR("Failed to get surface formats");
      swap_chain_support_.formats.clear();
      return;
    }
  }

#if _DEBUG
  for (const vk::SurfaceFormatKHR &format : swap_chain_support_.formats) {
    // print out supported surface formats
    GTRACE("Supported surface format:");
    GTRACE("  format: {}", vk::to_string(format.format));
    GTRACE("  colorSpace: {}", vk::to_string(format.colorSpace));
  }
#endif

  // check if surfaceFormat and colorSpace is supported
  bool found = false;
  for (const vk::SurfaceFormatKHR &format : swap_chain_support_.formats) {
    if (format.format == surface_format_ && format.colorSpace == color_space_) {
      found = true;
      GTRACE("Device supports targeted surface format & color space");
      GTRACE("Target Surface format: {}", vk::to_string(surface_format_));
      GTRACE("Target Color space: {}", vk::to_string(color_space_));
      break;
    }
  }
  if (!found) {
    GERROR("Surface format not found");
    return;
  }

  (void) physical_device.getSurfacePresentModesKHR(surface, &format_count, nullptr);
  if (format_count != 0) {
    swap_chain_support_.present_modes.resize(format_count);
    // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPresentModeKHR.html
    if (physical_device.getSurfacePresentModesKHR(surface, &format_count, swap_chain_support_.present_modes.data()) != vk::Result::eSuccess) {
      GERROR("Failed to get surface present modes");
      swap_chain_support_.present_modes.clear();
      return;
    }
  }

#if _DEBUG
  for (const vk::PresentModeKHR &mode : swap_chain_support_.present_modes) {
    // print out supported present modes
    GTRACE("Supported present mode:");
    GTRACE("  mode: {}", vk::to_string(mode));
  }
#endif

  // check if presentMode is supported
  found = false;
  for (auto &mode : swap_chain_support_.present_modes) {
    if (mode == present_mode_) {
      found = true;
      GINFO("Successfully set present mode to {}", vk::to_string(present_mode_));
      break;
    }
  }
  if (!found) {
    GWARN("Requested Present mode not found, defaulting to VK_PRESENT_MODE_FIFO_KHR");
    present_mode_ = vk::PresentModeKHR::eFifo;
  }
}

void VulkanSwapChain::CreateSwapChain() {
  const vk::SurfaceKHR surface = context_.GetSurface();
  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(surface != VK_NULL_HANDLE && device != VK_NULL_HANDLE, "Failed to get Vulkan logical device or surface");

  const uint32_t image_count = std::min(swap_chain_support_.capabilities.maxImageCount, swap_chain_support_.capabilities.minImageCount + 1);

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

  vk::SwapchainCreateInfoKHR swapchain_create_info = {};
  swapchain_create_info.sType = vk::StructureType::eSwapchainCreateInfoKHR;
  swapchain_create_info.surface = surface;
  swapchain_create_info.minImageCount = image_count;
  swapchain_create_info.imageFormat = surface_format_;
  swapchain_create_info.imageColorSpace = color_space_;
  swapchain_create_info.imageExtent = swap_chain_support_.capabilities.currentExtent;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = vk::ImageUsageFlags(vk::ImageUsageFlagBits::eColorAttachment);

  VK_ASSERT(context_.GetQueueIndexes().graphics_family.has_value() && context_.GetQueueIndexes().present_family.has_value(),
            "Failed to get graphics and present queue families");
  swap_chain_extent_ = swap_chain_support_.capabilities.currentExtent;

  QueueIndexes indexes = context_.GetQueueIndexes();
  uint32_t queue_family_indices[] = {indexes.graphics_family.value(), indexes.present_family.value()};
  // since there is a possibility that the graphics queue and the present queue are not the same index,
  // there will be two separate queue families acting on the swap chain.  We need to let the swap chain
  // know which queues will be used.
  // If they ARE THE SAME index, we can set imageSharingMode to VK_SHARING_MODE_EXCLUSIVE
  if (queue_family_indices[0] == queue_family_indices[1]) {
    swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
  } else {
    swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
  }

  swapchain_create_info.preTransform = swap_chain_support_.capabilities.currentTransform;
  swapchain_create_info.presentMode = present_mode_;
  swapchain_create_info.clipped = VK_TRUE;// if a window is rendered above the rendering window, it will be clipped
  swapchain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  swapchain_create_info.oldSwapchain = vk_swapchain_;// used during re-initialization from old to speed up creation

  VK_CHECK(device.createSwapchainKHR(&swapchain_create_info, nullptr, &vk_swapchain_), "Failed to create swap chain");
  GINFO("Successfully created swap chain");
}

void VulkanSwapChain::CreateImageViews() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  //  eventually at the end we will populare our list of swap chain frames with these vectors
  std::vector<vk::ImageView> image_views;
  std::vector<std::pair<vk::Image, vk::DeviceMemory>> depth_images;
  std::vector<vk::ImageView> depth_image_views;

  uint32_t image_count = 0;
  // get the number of images in the swap chain
  (void) device.getSwapchainImagesKHR(vk_swapchain_, &image_count, nullptr);
  std::vector<vk::Image> images;
  images.resize(image_count);
  depth_images.reserve(image_count);
  depth_image_views.reserve(image_count);
  (void) device.getSwapchainImagesKHR(vk_swapchain_, &image_count, images.data());

  // For each swapChaimImage, we need to construct an image view
  // create a std::vector that matches up with the number of images in the swapChainImages
  vk::ImageViewCreateInfo image_view_create_info = {};
  image_view_create_info.sType = vk::StructureType::eImageViewCreateInfo;
  image_view_create_info.format = surface_format_;
  image_view_create_info.viewType = vk::ImageViewType::e2D;             // VK_IMAGE_VIEW_TYPE_2D;
  image_view_create_info.components.r = vk::ComponentSwizzle::eIdentity;// VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = vk::ComponentSwizzle::eIdentity;// VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = vk::ComponentSwizzle::eIdentity;// VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = vk::ComponentSwizzle::eIdentity;
  image_view_create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;
  for (uint32_t i = 0; i < image_count; i++) {
    image_view_create_info.image = images[i];
    vk::ImageView image_view;
    VK_CHECK(device.createImageView(&image_view_create_info, nullptr, &image_view), "Failed to create image view");
    image_views.push_back(image_view);
  }
  GINFO("Successfully created image views - Count: {}", image_count);

  vk::Image depth_image = VK_NULL_HANDLE;
  vk::ImageView depth_image_view = VK_NULL_HANDLE;
  vk::DeviceMemory depth_image_memory = VK_NULL_HANDLE;

  // -- create depth buffer image
  vk::ImageCreateInfo depth_image_info = {};
  depth_image_info.sType = vk::StructureType::eImageCreateInfo;
  depth_image_info.flags = vk::ImageCreateFlags();
  depth_image_info.imageType = vk::ImageType::e2D;
  depth_image_info.format = vk::Format::eD32Sfloat;// TODO: should be configurable; grab it from the render pass input paramter struct?
  depth_image_info.extent.width = swap_chain_extent_.width;
  depth_image_info.extent.height = swap_chain_extent_.height;
  depth_image_info.extent.depth = 1;
  depth_image_info.mipLevels = 1;
  depth_image_info.arrayLayers = 1;
  depth_image_info.samples = vk::SampleCountFlagBits::e1;
  depth_image_info.tiling = vk::ImageTiling::eOptimal;
  depth_image_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
  // depth_image_info.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
  depth_image_info.sharingMode = vk::SharingMode::eExclusive;
  depth_image_info.initialLayout = vk::ImageLayout::eUndefined;

  // depth image view has same config except the aspect mask - borrowing the same struct with minor tweaks
  image_view_create_info.format = vk::Format::eD32Sfloat;// TODO: should be configurable; grab it from the render pass input paramter struct?
  image_view_create_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

  for (uint32_t i = 0; i < image_count; i++) {
    VK_CHECK(device.createImage(&depth_image_info, nullptr, &depth_image), "Failed to create depth image");

    // associate the image view with the new depth image
    image_view_create_info.image = depth_image;

    // Back the VkImage with memory
    vk::MemoryRequirements memory_requirements = {};
    device.getImageMemoryRequirements(depth_image, &memory_requirements);

    vk::MemoryAllocateInfo memory_allocate_info = {};
    memory_allocate_info.sType = vk::StructureType::eMemoryAllocateInfo;
    memory_allocate_info.pNext = nullptr;
    memory_allocate_info.allocationSize = memory_requirements.size;
    memory_allocate_info.memoryTypeIndex = VulkanUtils::FindMemoryTypeIndex(context_.GetVulkanPhysicalDevice(), memory_requirements.memoryTypeBits,
                                                                            vk::MemoryPropertyFlagBits::eDeviceLocal);

    VK_CHECK(device.allocateMemory(&memory_allocate_info, nullptr, &depth_image_memory), "Failed to allocate image memory for depth buffer");
    device.bindImageMemory(depth_image, depth_image_memory, 0);
    depth_images.emplace_back(depth_image, depth_image_memory);
    // depth_images.push_back(std::make_pair(depth_image, depth_image_memory));
    // the image_view create info here is tweaked to match with depth buffer
    VK_CHECK(device.createImageView(&image_view_create_info, nullptr, &depth_image_view), "Failed to create depth image view");
    depth_image_views.push_back(depth_image_view);
  }

  swap_chain_frames_.resize(image_count);

  for (uint32_t i = 0; i < image_count; i++) {
    swap_chain_frames_[i].image = images[i];
    swap_chain_frames_[i].image_view = image_views[i];

    auto [dpt_image, dpt_image_memory] = depth_images[i];
    swap_chain_frames_[i].depth_image = dpt_image;
    swap_chain_frames_[i].depth_image_memory = dpt_image_memory;
    swap_chain_frames_[i].depth_image_view = depth_image_views[i];
    swap_chain_frames_[i].depth_format = vk::Format::eD32Sfloat;// TODO: should be configurable; grab it from the render pass input paramter struct?
    swap_chain_frames_[i].depth_height = static_cast<int>(swap_chain_support_.capabilities.currentExtent.height);
    swap_chain_frames_[i].depth_width = static_cast<int>(swap_chain_support_.capabilities.currentExtent.width);
  }
}

void VulkanSwapChain::RebuildSwapChain(int width, int height) {
  const vk::SurfaceKHR surface = context_.GetSurface();
  const vk::PhysicalDevice physical_device = context_.GetVulkanPhysicalDevice();
  const vk::Device device = context_.GetVulkanLogicalDevice();
  vk::SwapchainKHR old_swap_chain = vk_swapchain_;

  // destroy image views, frame buffers
  DestroyFrames();

  const uint32_t image_count = std::min(swap_chain_support_.capabilities.maxImageCount, swap_chain_support_.capabilities.minImageCount + 1);
  std::vector<VkImageView> image_views;

  vk::SwapchainCreateInfoKHR swapchain_create_info = {};
  swapchain_create_info.sType = vk::StructureType::eSwapchainCreateInfoKHR;
  swapchain_create_info.surface = surface;
  swapchain_create_info.minImageCount = image_count;
  swapchain_create_info.imageFormat = surface_format_;
  swapchain_create_info.imageColorSpace = color_space_;
  swapchain_create_info.imageArrayLayers = 1;
  swapchain_create_info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

  // Poll surface
  if (physical_device.getSurfaceCapabilitiesKHR(surface, &swap_chain_support_.capabilities) != vk::Result::eSuccess) {
    GERROR("Failed to get surface capabilities");
    return;
  }

  const VkSurfaceCapabilitiesKHR cap = swap_chain_support_.capabilities;

  if (swapchain_create_info.minImageCount < cap.minImageCount) {
    swapchain_create_info.minImageCount = cap.minImageCount;
  } else if (cap.maxImageCount != 0 && swapchain_create_info.minImageCount > cap.maxImageCount) {
    swapchain_create_info.minImageCount = cap.maxImageCount;
  }

  if (cap.currentExtent.width == 0xFFFFFFFF) {
    swapchain_create_info.imageExtent.width = width;
    swapchain_create_info.imageExtent.height = height;
  } else {
    swapchain_create_info.imageExtent.width = width = static_cast<int>(cap.currentExtent.width);
    swapchain_create_info.imageExtent.height = height = static_cast<int>(cap.currentExtent.height);
  }

  VK_ASSERT(context_.GetQueueIndexes().graphics_family.has_value() && context_.GetQueueIndexes().present_family.has_value(),
            "Swap chain requires both graphics and present queues");

  QueueIndexes indexes = context_.GetQueueIndexes();
  uint32_t queue_family_indices[] = {indexes.graphics_family.value(), indexes.present_family.value()};
  // since there is a possibility that the graphics queue and the present queue are not the same index,
  // there will be two separate queue families acting on the swap chain.  We need to let the swap chain
  // know which queues will be used.
  // If they ARE THE SAME index, we can set imageSharingMode to VK_SHARING_MODE_EXCLUSIVE
  if (queue_family_indices[0] != queue_family_indices[1]) {
    swapchain_create_info.imageSharingMode = vk::SharingMode::eConcurrent;
    swapchain_create_info.queueFamilyIndexCount = 2;
    swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
  } else {
    swapchain_create_info.imageSharingMode = vk::SharingMode::eExclusive;
    swapchain_create_info.queueFamilyIndexCount = 0;
    swapchain_create_info.pQueueFamilyIndices = nullptr;
  }

  swapchain_create_info.preTransform = swap_chain_support_.capabilities.currentTransform;
  swapchain_create_info.presentMode = present_mode_;
  swapchain_create_info.clipped = VK_TRUE;// if a window is rendered above the rendering window, it will be clipped
  swapchain_create_info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
  // used during re-initialization from old to speed up creation
  swapchain_create_info.oldSwapchain = old_swap_chain == VK_NULL_HANDLE ? VK_NULL_HANDLE : old_swap_chain;

  VK_CHECK(device.createSwapchainKHR(&swapchain_create_info, nullptr, &vk_swapchain_), "Failed to re-create swap chain");

  // FIXME: There's probably another way to set this while doing the PopulateSwapChain function
  swap_chain_extent_ = swap_chain_support_.capabilities.currentExtent;

  if (old_swap_chain != VK_NULL_HANDLE) {
    // destroy old swap chain
    device.destroySwapchainKHR(old_swap_chain, nullptr);
  }

  GINFO("Successfully regenerated swap chain");

  CreateImageViews();
  CreateFrameBuffers();
  CreateDescriptorResources();
}

void VulkanSwapChain::DestroyFrames() {
  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get logical device");

  for (SwapChainFrame &swap_chain_frame : swap_chain_frames_) {
    // destroy image views
    if (swap_chain_frame.image_view != VK_NULL_HANDLE) {
      device.destroy(swap_chain_frame.image_view, nullptr);
      swap_chain_frame.image_view = VK_NULL_HANDLE;
    }
    if (swap_chain_frame.depth_image_view != VK_NULL_HANDLE) {
      device.destroy(swap_chain_frame.depth_image_view);
      swap_chain_frame.depth_image_view = VK_NULL_HANDLE;
    }

    // do not destroy images associated with swap chain, let destroySwapChain handle that

    // destroy frame buffer
    if (swap_chain_frame.frame_buffer != VK_NULL_HANDLE) {
      device.destroy(swap_chain_frame.frame_buffer, nullptr);
      swap_chain_frame.frame_buffer = VK_NULL_HANDLE;
    }

    // destroy ubo
    if (swap_chain_frame.camera_data_buffer.buffer != VK_NULL_HANDLE) {
      device.unmapMemory(swap_chain_frame.camera_data_buffer.buffer_memory);
      device.freeMemory(swap_chain_frame.camera_data_buffer.buffer_memory, nullptr);
      device.destroy(swap_chain_frame.camera_data_buffer.buffer, nullptr);
      swap_chain_frame.camera_data_mapped = nullptr;
      swap_chain_frame.camera_data_buffer.buffer = VK_NULL_HANDLE;
    }

    // destroy model matrices
    if (swap_chain_frame.model_matrices_buffer.buffer != VK_NULL_HANDLE) {
      device.unmapMemory(swap_chain_frame.model_matrices_buffer.buffer_memory);
      device.freeMemory(swap_chain_frame.model_matrices_buffer.buffer_memory, nullptr);
      device.destroy(swap_chain_frame.model_matrices_buffer.buffer, nullptr);
      swap_chain_frame.model_matrices_mapped = nullptr;
      swap_chain_frame.model_matrices_buffer.buffer = VK_NULL_HANDLE;
    }

    // destroy depth buffer
    if (swap_chain_frame.depth_image != VK_NULL_HANDLE) {
      device.destroyImage(swap_chain_frame.depth_image, nullptr);
      swap_chain_frame.depth_image = VK_NULL_HANDLE;

      device.destroy(swap_chain_frame.depth_image_view, nullptr);
      swap_chain_frame.depth_image_view = nullptr;

      // when using bindimagememory, we do not need to unmap it; just free it.
      device.freeMemory(swap_chain_frame.depth_image_memory, nullptr);
      swap_chain_frame.depth_image_memory = VK_NULL_HANDLE;

      swap_chain_frame.depth_height = swap_chain_frame.depth_width = -1;
    }
  }
  swap_chain_frames_.clear();
}

void VulkanSwapChain::CreateFrameBuffers() {
  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get logical device");

  vk::FramebufferCreateInfo framebuffer_create_info = {};
  framebuffer_create_info.sType = vk::StructureType::eFramebufferCreateInfo;
  framebuffer_create_info.renderPass = context_.GetVulkanRenderPass().GetVkRenderPass();
  framebuffer_create_info.width = swap_chain_support_.capabilities.currentExtent.width;
  framebuffer_create_info.height = swap_chain_support_.capabilities.currentExtent.height;
  framebuffer_create_info.layers = 1;

  for (SwapChainFrame &swap_chain_frame : swap_chain_frames_) {
    const vk::ImageView attachments[] = {swap_chain_frame.image_view, swap_chain_frame.depth_image_view};
    framebuffer_create_info.pAttachments = attachments;
    framebuffer_create_info.attachmentCount = sizeof(attachments) / sizeof(vk::ImageView);
    VK_CHECK(device.createFramebuffer(&framebuffer_create_info, nullptr, &swap_chain_frame.frame_buffer), "Failed to create frame buffers");
  }
  GINFO("Successfully created frame buffers");
}

void VulkanSwapChain::Destroy() {
  DestroyFrames();

  const vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get logical device");

  if (vk_swapchain_ != VK_NULL_HANDLE) {
    // destroy swap chain
    device.destroy(vk_swapchain_, nullptr);
    vk_swapchain_ = VK_NULL_HANDLE;
  }
}
void VulkanSwapChain::CreateDescriptorResources() {
  const vk::Device device = context_.GetVulkanLogicalDevice();

  VulkanUtils::BufferInputParams uniform_params = {};
  uniform_params.device = context_.GetVulkanLogicalDevice();
  uniform_params.physical_device = context_.GetVulkanPhysicalDevice();
  uniform_params.buffer_usage = vk::BufferUsageFlagBits::eUniformBuffer;
  uniform_params.memory_property_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  uniform_params.size = sizeof(UniformBufferObject);

  VulkanUtils::BufferInputParams storage_params = {};
  storage_params.device = context_.GetVulkanLogicalDevice();
  storage_params.physical_device = context_.GetVulkanPhysicalDevice();
  storage_params.buffer_usage = vk::BufferUsageFlagBits::eStorageBuffer;
  storage_params.memory_property_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  storage_params.size = sizeof(glm::mat4) * 1024;

  for (SwapChainFrame &frame : swap_chain_frames_) {
    frame.camera_data_buffer = VulkanUtils::CreateBuffer(uniform_params);
    VK_CHECK(device.mapMemory(frame.camera_data_buffer.buffer_memory, 0, sizeof(UniformBufferObject), {}, &frame.camera_data_mapped),
             "Failed to map memory for camera data");

    frame.model_matrices.resize(1024, glm::mat4(1.0f));
    frame.model_matrices_buffer = VulkanUtils::CreateBuffer(storage_params);
    VK_CHECK(device.mapMemory(frame.model_matrices_buffer.buffer_memory, 0, sizeof(glm::mat4) * 1024, {}, &frame.model_matrices_mapped),
             "Failed to map memory for model matrices data");
  }
}

void VulkanSwapChain::UpdateDescriptorResources() {
  const vk::Device device = context_.GetVulkanLogicalDevice();

  // Provided by VK_VERSION_1_0
  //  typedef struct VkDescriptorBufferInfo {
  //    VkBuffer        buffer;
  //    VkDeviceSize    offset;
  //    VkDeviceSize    range;
  //  } VkDescriptorBufferInfo;

  for (SwapChainFrame &frame : swap_chain_frames_) {
    // Similar to UBO, we need to parse vk::DescriptorSet into its raw form.
    // This is where the uniform buffer descriptor comes in aka vk::DescriptorSetInfo
    frame.uniform_buffer_descriptor.buffer = frame.camera_data_buffer.buffer;
    frame.uniform_buffer_descriptor.offset = 0;
    frame.uniform_buffer_descriptor.range = sizeof(UniformBufferObject);

    frame.model_matrices_buffer_descriptor.buffer = frame.model_matrices_buffer.buffer;
    frame.model_matrices_buffer_descriptor.offset = 0;
    frame.model_matrices_buffer_descriptor.range = sizeof(glm::mat4) * 1024;

    // Provided by VK_VERSION_1_0
    //    typedef struct VkWriteDescriptorSet {
    //      VkStructureType                  sType;
    //      const void*                      pNext;
    //      VkDescriptorSet                  dstSet;
    //      uint32_t                         dstBinding;
    //      uint32_t                         dstArrayElement;
    //      uint32_t                         descriptorCount;
    //      VkDescriptorType                 descriptorType;
    //      const VkDescriptorImageInfo*     pImageInfo;
    //      const VkDescriptorBufferInfo*    pBufferInfo;
    //      const VkBufferView*              pTexelBufferView;
    //    } VkWriteDescriptorSet;
    const vk::DescriptorSet dst_set = context_.GetVulkanDescriptorPool().GetDescriptorSet(DescriptorPoolType::FRAME)[0];
    // frame descriptor set has two bindings
    // binding 0: camera data
    // binding 1: model matrices

    vk::WriteDescriptorSet write_descriptor_set;
    write_descriptor_set.sType = vk::StructureType::eWriteDescriptorSet;
    write_descriptor_set.dstSet = dst_set;
    write_descriptor_set.dstBinding = 0;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = vk::DescriptorType::eUniformBuffer;
    write_descriptor_set.pBufferInfo = &frame.uniform_buffer_descriptor;

    device.updateDescriptorSets(write_descriptor_set, nullptr);

    write_descriptor_set.dstSet = dst_set;
    write_descriptor_set.dstBinding = 1;
    write_descriptor_set.dstArrayElement = 0;
    write_descriptor_set.descriptorCount = 1;
    write_descriptor_set.descriptorType = vk::DescriptorType::eStorageBuffer;
    write_descriptor_set.pBufferInfo = &frame.model_matrices_buffer_descriptor;
    device.updateDescriptorSets(write_descriptor_set, nullptr);
  }
}
}// namespace glaceon
