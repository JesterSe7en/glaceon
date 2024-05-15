#ifndef GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_
#define GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_

#include "Base.h"

namespace glaceon {

enum MemoryTag {
  MEMORY_TAG_UNKNOWN,
  MEMORY_TAG_ARRAY,
  MEMORY_TAG_DYNARRAY,
  MEMORY_TAG_DICT,
  MEMORY_TAG_STRING,
  MEMORY_TAG_TEXTURE,

  MEMORY_TAG_MAX_TAGS
};

class MemorySubsystem {
 public:
  MemorySubsystem();
  ~MemorySubsystem();

  static void *GAllocate(uint64_t size, MemoryTag tag);
  static void GFree(void *mem_block, uint64_t size, MemoryTag tag);
  static void *GZeroMemory(void *mem_block, uint64_t size);
  static void *GCopyMemory(void *dest, const void *src, uint64_t size);
  static void *GSetMemory(void *dest, int value, uint64_t size);

  static void PrintStats();
};

class PoolAllocator {
 public:
  PoolAllocator(size_t objSize, uint8_t alignment, size_t size, void *start);
  ~PoolAllocator();

  void *Allocate(size_t size, uint8_t alignment);
  void Deallocate(void *ptr);

 private:
  PoolAllocator(const PoolAllocator &) = delete;
  PoolAllocator &operator=(const PoolAllocator &) = delete;

  uint8_t objectAlignment;
  size_t objectSize;
  void **free_list_;
  size_t size_;
  size_t used_memory_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_
