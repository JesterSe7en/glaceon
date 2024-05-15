#ifndef GLACEON_GLACEON_CORE_LINEARALLOCATOR_H_
#define GLACEON_GLACEON_CORE_LINEARALLOCATOR_H_

#include "Base.h"
#include "Interface_Allocator.h"

namespace glaceon {
class LinearAllocator : public IAllocator {
 public:
  LinearAllocator(size_t size, void *start);
  ~LinearAllocator() override;

  void *Allocate(size_t size, uint8_t alignment) override;
  void Deallocate(void *ptr) override;
  void Clear();

  // Deleting the copy constructor to prevent copying of LinearAllocator objects
  LinearAllocator(const LinearAllocator &) = delete;
  // Deleting the copy assignment operator to prevent assigning values to another LinearAllocator object
  LinearAllocator &operator=(const LinearAllocator &) = delete;

  void *current_pos_;
};
}// namespace glaceon
#endif//GLACEON_GLACEON_CORE_LINEARALLOCATOR_H_
