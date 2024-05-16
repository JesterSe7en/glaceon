#ifndef GLACEON_GLACEON_CORE_FREELISTALLOCATOR_H_
#define GLACEON_GLACEON_CORE_FREELISTALLOCATOR_H_

namespace glaceon {

class FreeListAllocator {
 public:
  FreeListAllocator(size_t size, void *start);
  ~FreeListAllocator();

  void *Allocate(size_t size, uint8_t alignment);
  void Deallocate(void *ptr);

  FreeListAllocator(const FreeListAllocator &) = delete;
  FreeListAllocator &operator=(const FreeListAllocator &) = delete;

 private:
  struct AllocationHeader {
    size_t size;
    uint8_t adjustment;// how many bytes to adjust to align the next allocation
  };
  struct FreeBlock {
    size_t size;
    FreeBlock *next;
  };

  FreeBlock *free_blocks_;
  size_t size_;
  size_t used_memory_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_FREELISTALLOCATOR_H_
