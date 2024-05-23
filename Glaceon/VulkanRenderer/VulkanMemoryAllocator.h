#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_

#include <vma/vk_mem_alloc.h>

namespace glaceon {

class VulkanContext;

class VulkanMemoryAllocator {

 public:
  explicit VulkanMemoryAllocator(VulkanContext &context);
  ~VulkanMemoryAllocator();

  void Initialize();

 private:
  VulkanContext &context_;

  VmaAllocator allocator_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VULKANRENDERER_VULKANMEMORYALLOCATOR_H_
