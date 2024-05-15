#ifndef GLACEON_GLACEON_CORE_STACKALLOCATOR_H_
#define GLACEON_GLACEON_CORE_STACKALLOCATOR_H_

#include "Interface_Allocator.h"

namespace glaceon {

class StackAllocator : public IAllocator {
 public:
  StackAllocator(size_t size, void *start);
  ~StackAllocator() override;

  void *Allocate(size_t size, uint8_t alignment) override;
  void Deallocate(void *ptr) override;
  void Clear();

  StackAllocator(const StackAllocator &) = delete;
  StackAllocator &operator=(const StackAllocator &) = delete;

  void *current_pos_;
  void *start_;
  size_t size_;
  size_t used_memory_;

#if _DEBUG
  void *prev_position_;
#endif// _DEBUG

  struct AllocationHeader {
#if _DEBUG
    void *prev_address_;
#endif// _DEBUG

    uint8_t adjustment;// how many bytes to adjust to align the next allocation
  };
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_STACKALLOCATOR_H_
