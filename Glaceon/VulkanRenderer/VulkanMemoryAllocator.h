#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace glaceon {

class VulkanContext;

class VulkanMemoryAllocator {

 public:
  explicit VulkanMemoryAllocator(VulkanContext &context);
  ~VulkanMemoryAllocator();

 private:
  VmaAllocator allocator_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_
