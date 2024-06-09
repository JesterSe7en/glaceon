#ifndef GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_
#define GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_

#include "../Base.h"
#include "StackAllocator.h"


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

  // uses malloc
  static void *GAllocate(uint64_t size, MemoryTag tag);
  // uses calloc
  static void *GAllocate(uint64_t num, uint64_t size_of_obj, size_t align, MemoryTag tag);
  static void GFree(void *mem_block, uint64_t size, MemoryTag tag);
  static void *GZeroMemory(void *mem_block, uint64_t size);
  static void *GCopyMemory(void *dest, const void *src, uint64_t size);
  static void *GSetMemory(void *dest, int value, uint64_t size);

  static void PrintStats();
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_MEMORYSUBSYSTEM_H_
