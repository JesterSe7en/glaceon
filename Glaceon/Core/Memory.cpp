#include "Memory.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "../Utils.h"
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

// -------------------------- LINEAR ALLOCATOR --------------------------

// Initializes the LinearAllocator with the given size and start address.
// Typically, malloc is called prior to wrapping the malloc block into a LinearAllocator.

// e.g.
// 	size_t memory_size = 1024ULL * 1024 * 1024; //1GB
//	_memory            = malloc(memory_size);
// 	_main_allocator = new (_memory)LinearAllocator(memory_size - sizeof(FreeListAllocator),

LinearAllocator::LinearAllocator(size_t size, void *start) {
  current_pos_ = start;
  start_ = start;
  size_ = size;
  used_memory_ = 0;
  assert(size > 0);// Ensure that size is greater than 0
}

LinearAllocator::~LinearAllocator() { current_pos_ = nullptr; }

/**
 * @brief Allocates a block of memory from the LinearAllocator.
 *
 * @param size The size of the memory block to allocate.
 * @param alignment The alignment of the memory block.
 * @return void* A pointer to the allocated memory block, or nullptr if the allocation failed.
 */
void *LinearAllocator::Allocate(size_t size, uint8_t alignment) {
  if (size == 0) return nullptr;

  uint8_t adjustment = AlignSize(current_pos_, alignment);
  if (used_memory_ + size + adjustment > size_) return nullptr;// no more memory available with given size and alignment

  uintptr_t aligned_address = reinterpret_cast<uintptr_t>(current_pos_) + adjustment;
  used_memory_ += size + adjustment;
  current_pos_ = reinterpret_cast<uintptr_t *>(aligned_address + size);// Move current_pos_ by the allocated size

  return reinterpret_cast<void *>(aligned_address);
}

/**
 * @brief Clears the LinearAllocator, resetting the current position to the start and setting the used memory to 0.
 *
 * This function is used to reset the LinearAllocator to its initial state. It is typically called when you want to reuse the LinearAllocator for a new allocation.
 */
void LinearAllocator::Clear() {
  current_pos_ = start_;
  used_memory_ = 0;
}

// -------------------------- STACK ALLOCATOR --------------------------
StackAllocator::StackAllocator(size_t size, void *start) {
  assert(size > 0);// Ensure that size is greater than 0

#if _DEBUG
  prev_position_ = nullptr;
#endif// _DEBUG
  prev_position_ = nullptr;

  current_pos_ = start;
  start_ = start;
  size_ = size;
  used_memory_ = 0;
}

StackAllocator::~StackAllocator() {
#if _DEBUG
  prev_position_ = nullptr;
#endif// _DEBUG
  prev_position_ = nullptr;
  current_pos_ = nullptr;
}

void *StackAllocator::Allocate(size_t size, uint8_t alignment) {
  if (size == 0) return nullptr;

  uint8_t adjustment = AlignSize(current_pos_, alignment);
  if (used_memory_ + size + adjustment > size_) return nullptr;

  void *align_address = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(current_pos_) + adjustment);

  // Prepend the AllocationHeader to the current position
  auto *header = (AllocationHeader *) (&align_address - sizeof(AllocationHeader));
  header->adjustment = adjustment;

#if _DEBUG
  header->prev_address_ = prev_position_;
  prev_position_ = align_address;
#endif// _DEBUG

  current_pos_ = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(align_address) + size);// put pointer to next free space
  used_memory_ += size + adjustment;

  return align_address;
}

void StackAllocator::Deallocate(void *ptr) {
  assert(ptr == prev_position_);

  auto *header = reinterpret_cast<AllocationHeader *>(reinterpret_cast<uintptr_t>(ptr) - sizeof(AllocationHeader));
  used_memory_ -= reinterpret_cast<uintptr_t>(current_pos_) - header->adjustment;
  current_pos_ = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(ptr) - header->adjustment);// set current position to next free space

#if _DEBUG
  prev_position_ = header->prev_address_;
#endif// _DEBUG
}

// -------------------------- FREE-LIST ALLOCATOR --------------------------
FreeListAllocator::FreeListAllocator(size_t size, void *start) {
  assert(size > 0);
  free_blocks_->size = size;
  free_blocks_->next = nullptr;
}

FreeListAllocator::~FreeListAllocator() { free_blocks_ = nullptr; }

void *FreeListAllocator::Allocate(size_t size, uint8_t alignment) {
  assert(size != 0 && alignment != 0);
  FreeBlock *prev_free_block = nullptr;
  FreeBlock *free_block = free_blocks_;

  while (free_block != nullptr) {
    if (free_block->size >= size) {
      prev_free_block = free_block;
      free_block = free_block->next;
      continue;
    }
  }
}

}// namespace glaceon
