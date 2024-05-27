#include "VulkanMemoryAllocator.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "../Core/Logger.h"
#include "VulkanBase.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanMemoryAllocator::VulkanMemoryAllocator(VulkanContext &context) : context_(context), allocator_(nullptr) {}
VulkanMemoryAllocator::~VulkanMemoryAllocator() = default;

void VulkanMemoryAllocator::Initialize() {
  VmaAllocatorCreateInfo allocator_create_info = {};
  allocator_create_info.device = context_.GetVulkanLogicalDevice();
  allocator_create_info.instance = context_.GetVulkanInstance();
  allocator_create_info.physicalDevice = context_.GetVulkanPhysicalDevice();
  VK_CHECK(vmaCreateAllocator(&allocator_create_info, &allocator_), "Failed to create VulkanMemoryAllocator");
}
}// namespace glaceon
