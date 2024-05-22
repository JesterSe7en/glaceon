#include "VulkanMemoryAllocator.h"

#include "../Core/Logger.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanContext &context) {
  VmaAllocatorCreateInfo allocator_create_info = {};
  allocator_create_info.device = context.GetVulkanLogicalDevice();
  allocator_create_info.instance = context.GetVulkanInstance();
  allocator_create_info.physicalDevice = context.GetVulkanPhysicalDevice();
  if (vmaCreateAllocator(&allocator_create_info, &allocator_) != VK_SUCCESS) {
    GERROR("Failed to create VulkanMemoryAllocator");
    exit(0);
  };
}
VulkanMemoryAllocator::~VulkanMemoryAllocator() {}
}// namespace glaceon