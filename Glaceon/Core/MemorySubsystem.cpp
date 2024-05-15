#include "MemorySubsystem.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "../Utils.h"
#include "LinearAllocator.h"
#include "Logger.h"

namespace glaceon {

struct memory_stats {
  uint64_t total_allocated;
  uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static std::string tag_names[MEMORY_TAG_MAX_TAGS] = {"Unknown", "Array", "DynArray", "Dict", "String", "Texture"};

static memory_stats stats_ = {0, {0, 0, 0, 0, 0, 0}};

MemorySubsystem::MemorySubsystem() {
  // zero out memory stats
  GSetMemory(&stats_, 0, sizeof(stats_));
}

MemorySubsystem::~MemorySubsystem() {}

void *MemorySubsystem::GAllocate(uint64_t size, MemoryTag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) { GWARN("GAllocate called with MEMORY_TAG_UNKNOWN.  Specifying a tag is recommended."); }

  // update stats
  stats_.total_allocated += size;
  GTRACE("Total allocated: {}", stats_.total_allocated);
  stats_.tagged_allocations[tag] += size;
  GTRACE("Tagged allocations: {}", tag_names[tag]);

  // TODO: allow for alignment bool?
  return GZeroMemory(malloc(size), size);
}

void MemorySubsystem::GFree(void *mem_block, uint64_t size, MemoryTag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) { GWARN("GFree called with MEMORY_TAG_UNKNOWN.  Specifying a tag is recommended."); }

  // update stats
  stats_.total_allocated -= size;
  stats_.tagged_allocations[tag] -= size;

  // TODO: allow for alignment bool?
  free(mem_block);
}

void *MemorySubsystem::GZeroMemory(void *mem_block, uint64_t size) { return memset(mem_block, 0, size); }

void *MemorySubsystem::GCopyMemory(void *dest, const void *src, uint64_t size) { return memcpy(dest, src, size); }

void *MemorySubsystem::GSetMemory(void *dest, int value, uint64_t size) { return memset(dest, value, size); }

// TODO: convert this to return a string
void MemorySubsystem::PrintStats() {
  GTRACE("System memory in use:");

  // print out memory stats for each memory tag type
  const uint64_t kKib = 1024;
  const uint64_t kMib = kKib * 1024;
  const uint64_t kGib = kMib * 1024;

  for (uint32_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
    if (stats_.tagged_allocations[i] >= kGib) {
      GTRACE("{}: {:03.2f} {}", tag_names[i], stats_.tagged_allocations[i] / static_cast<float>(kGib), "GiB");
    } else if (stats_.tagged_allocations[i] >= kMib) {
      GTRACE("{}: {:03.2f} {}", tag_names[i], stats_.tagged_allocations[i] / static_cast<float>(kMib), "MiB");
    } else if (stats_.tagged_allocations[i] >= kKib) {
      GTRACE("{}: {:03.2f} {}", tag_names[i], stats_.tagged_allocations[i] / static_cast<float>(kKib), "KiB");
    } else {
      GTRACE("{}: {:03.2f} {}", tag_names[i], static_cast<float>(stats_.tagged_allocations[i]), "B");
    }
  }
}

/**
 * @brief Constructor for PoolAllocator.
 * 
 * @param objSize Size of each object in the pool.
 * @param alignment Alignment requirement for each object.
 * @param size Total size of the memory pool.
 * @param start Starting address of the memory pool.
 */
PoolAllocator::PoolAllocator(size_t objSize, uint8_t alignment, size_t size, void *start) : objectAlignment(alignment), objectSize(objSize) {
  // Calculate adjustment for alignment
  uint8_t adjustment = AlignSize(start, alignment);

  // Set the free list pointer
  free_list_ = reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(start) + adjustment);

  // Calculate the number of objects that can fit in the pool
  size_t numObjects = (size - adjustment) / objSize;

  // Initialize the free list
  void **p = free_list_;
  for (size_t i = 0; i < numObjects - 1; i++) {
    *p = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(start) + adjustment + i * objSize);
    p++;
  }

  // Set the last element of the free list to nullptr
  *p = nullptr;
}

PoolAllocator::~PoolAllocator() { free_list_ = nullptr; }

void *PoolAllocator::Allocate(size_t size, uint8_t alignment) {
  // Check if the size, alignment, or free list is invalid
  if (size == 0 || alignment == 0 || size != objectSize || alignment != objectAlignment || free_list_ == nullptr) {
    return nullptr;// Return nullptr as allocation is not possible
  }

  // Get the pointer to the next free block
  void *ptr = *free_list_;

  // Update the free list pointer to the next free block
  free_list_ = (void **) *free_list_;

  // Increase the used memory by the allocated size
  used_memory_ += size;

  return ptr;// Return the allocated memory block
}

void PoolAllocator::Deallocate(void *ptr) {
  // Set the next pointer of the deallocated block to the current free list
  *((void **) ptr) = free_list_;
  // Update the free list to point to the deallocated block, making it the new head of the free list
  free_list_ = (void **) ptr;
  // Decrease the used memory by the size of the deallocated block
  used_memory_ -= objectSize;
}

}// namespace glaceon
