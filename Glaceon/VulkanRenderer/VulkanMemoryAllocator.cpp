#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "../Core/Logger.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanContext &context) : context_(context) {}
VulkanMemoryAllocator::~VulkanMemoryAllocator() {}

void VulkanMemoryAllocator::Initialize() {
  VmaAllocatorCreateInfo allocator_create_info = {};
  allocator_create_info.device = context_.GetVulkanLogicalDevice();
  allocator_create_info.instance = context_.GetVulkanInstance();
  allocator_create_info.physicalDevice = context_.GetVulkanPhysicalDevice();
  if (vmaCreateAllocator(&allocator_create_info, &allocator_) != VK_SUCCESS) {
    GERROR("Failed to create VulkanMemoryAllocator");
    exit(0);
  };
}
}// namespace glaceon