#ifndef GLACEON_GLACEON_CORE_MEMORY_H_
#define GLACEON_GLACEON_CORE_MEMORY_H_

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
  static uintptr_t AlignAddress(uintptr_t address, uint32_t alignment);

  static void PrintStats();
};

class LinearAllocator {
 public:
  LinearAllocator(size_t size, void *start);
  ~LinearAllocator();

  void *Allocate(size_t size, uint8_t alignment);
  void Clear();

 private:
  uintptr_t AlignAddress(uintptr_t address, uint32_t alignment);
  void *current_pos_;
  void *start_;
  size_t size_;
  size_t used_memory_;
};

class StackAllocator {
 public:
  StackAllocator(size_t size, void *start);
  ~StackAllocator();

  void *Allocate(size_t size, uint8_t alignment);
  void Deallocate(void *ptr);
  void Clear();

 private:
  uintptr_t AlignAddress(uintptr_t address, uint32_t alignment);
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

    uint8_t alignment;
  };
};

}// namespace glaceon

#endif//GLACEON_GLACEON_CORE_MEMORY_H_
