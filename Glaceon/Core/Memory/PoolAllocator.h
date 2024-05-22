#ifndef GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_
#define GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_

#include "../Base.h"
namespace glaceon {
class PoolAllocator {
 public:
  PoolAllocator(size_t obj_size, uint8_t alignment, size_t size, void *start);
  ~PoolAllocator();

  void *Allocate(unsigned long long int size, unsigned char alignment);
  void Deallocate(void *ptr);

  PoolAllocator(const PoolAllocator &) = delete;
  PoolAllocator &operator=(const PoolAllocator &) = delete;

 private:
  unsigned char objectAlignment;
  unsigned long long int objectSize;
  void **free_list_;
  unsigned long long int size_;
  unsigned long long int used_memory_;
};
}// namespace glaceon
#endif//GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_CPP_POOLALLOCATOR_H_
