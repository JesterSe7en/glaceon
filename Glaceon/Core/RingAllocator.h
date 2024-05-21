#ifndef GLACEON_GLACEON_CORE_RINGALLOCATOR_H_
#define GLACEON_GLACEON_CORE_RINGALLOCATOR_H_

#include "Interface_Allocator.h"

namespace glaceon {

class RingAllocator : public IAllocator {
 public:
  RingAllocator(size_t size, void *start);
  ~RingAllocator() override;

  void *Allocate(size_t size, uint8_t alignment) override;
  void Deallocate(void *ptr) override;

  RingAllocator(const RingAllocator &) = delete;
  RingAllocator &operator=(const RingAllocator &) = delete;

 private:
  uintptr_t GetAvailableSpace(uintptr_t address);

  uintptr_t head_, tail_;
  bool is_full_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_RINGALLOCATOR_H_
