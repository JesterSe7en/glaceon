#ifndef GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_
#define GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_

#include <cstdint>

#include "../Base.h"
namespace glaceon {
class PoolAllocator {
 public:
  PoolAllocator(size_t obj_size, uint8_t alignment, size_t size, void *start);
  ~PoolAllocator();

  void *Allocate(size_t size, uint8_t alignment);
  void Deallocate(void *ptr);

  PoolAllocator(const PoolAllocator &) = delete;
  PoolAllocator &operator=(const PoolAllocator &) = delete;

 private:
  uint8_t object_alignment_;
  size_t object_size_;
  void **free_list_;
  size_t size_;
  size_t used_memory_;
};
}// namespace glaceon
#endif//GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_
