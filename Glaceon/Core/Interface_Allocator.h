#ifndef GLACEON_GLACEON_CORE_INTERFACE_ALLOCATOR_H_
#define GLACEON_GLACEON_CORE_INTERFACE_ALLOCATOR_H_

namespace glaceon {

class IAllocator {
 public:
  virtual ~IAllocator() = 0;

  virtual void *Allocate(size_t size, uint8_t alignment) = 0;
  virtual void Deallocate(void *ptr) = 0;

  void *GetStart() const { return start_; }
  size_t GetSize() const { return size_; }
  size_t GetUsedMemory() const { return used_memory_; }
  size_t GetNumAllocations() const { return num_allocations_; }

 protected:
  void *start_;
  size_t size_;
  size_t used_memory_;
  size_t num_allocations_;
};

}// namespace glaceon

#endif// GLACEON_GLACEON_CORE_INTERFACE_ALLOCATOR_H_