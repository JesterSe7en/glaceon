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
  used_memory_ = 0;
  free_blocks_ = (FreeBlock *) start;
  free_blocks_->size = size;
  free_blocks_->next = nullptr;
}

FreeListAllocator::~FreeListAllocator() { free_blocks_ = nullptr; }

void *FreeListAllocator::Allocate(size_t size, uint8_t alignment) {
  assert(size != 0 && alignment != 0);

  /*
   * The free list is a singly-linked list of free blocks,
   * where each free block has a size and a pointer to the next free block.
   *
   * The free block structure is defined in the header, and consists of:
   * - size_t size;                  // size of the free block
   * - FreeBlock *next;              // pointer to the next free block
   */

  // Keep track of the previous free block in the list
  FreeBlock *pPrev_free_block = nullptr;

  // Start at the beginning of the free list
  FreeBlock *pFree_block = free_blocks_;

  /*
   * Iterate through the free list, until we find a free block
   * that is big enough to hold the requested allocation.
   */
  while (pFree_block != nullptr) {

    // Calculate the adjustment needed for alignment
    uint8_t adjustment = AlignSize(pFree_block, alignment);

    // Calculate the total size needed for the requested allocation
    size_t total_size = size + adjustment;

    // Check if the free block is big enough
    if (pFree_block->size <= total_size) {
      // The free block is not big enough, so continue to the next one
      pPrev_free_block = pFree_block;
      pFree_block = pFree_block->next;
      continue;
    }

    // Now check if there is enough room for the AllocationHeader
    if (pFree_block->size - total_size >= sizeof(AllocationHeader)) {
      // There is enough room for the AllocationHeader

      /*
       * Use the free block as the allocated block by setting the requested
       * allocation size to the size of the free block instead of creating
       * a new FreeBlock.
       */
      total_size = pFree_block->size;

      /*
       * Remove the free block from the free list by updating the next
       * pointer of the previous free block (or the free list if the current
       * block is the first in the list).
       */
      if (pPrev_free_block != nullptr) {
        pPrev_free_block->next = pFree_block->next;
      } else {
        // the current block is the first in the list
        free_blocks_ = pFree_block->next;
      }
    } else {
      // There is not enough room for the AllocationHeader, so create a new FreeBlock

      /*
       * Create a new FreeBlock after the current one, and point the next
       * pointer of the current FreeBlock to the new FreeBlock.
       */
      auto *next_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<uintptr_t>(pFree_block) + total_size);
      next_block->size = pFree_block->size - total_size;
      next_block->next = pFree_block->next;

      /*
       * Update the next pointer of the previous free block (or the free
       * list if the current block is the first in the list).
       */
      if (pPrev_free_block != nullptr) {
        pPrev_free_block->next = next_block;
      } else {
        free_blocks_ = next_block;
      }
    }

    // Calculate the aligned address of the allocated block
    uintptr_t aligned_address = reinterpret_cast<uintptr_t>(pFree_block) + adjustment;

    // Prepend the AllocationHeader to the allocated block
    auto *header = reinterpret_cast<AllocationHeader *>(aligned_address - sizeof(AllocationHeader));
    header->size = total_size;
    header->adjustment = adjustment;

    // Update the used memory
    used_memory_ += total_size;

    // Return a pointer to the allocated block
    return reinterpret_cast<void *>(aligned_address);
  }

  // No free block was big enough, so return nullptr
  return nullptr;
}

// Deallocate function to release memory allocated by the FreeListAllocator
void FreeListAllocator::Deallocate(void *ptr) {
  // Check if the pointer is nullptr, if so, return immediately
  if (ptr == nullptr) { return; }

  // Get the AllocationHeader based on the provided pointer
  auto *header = reinterpret_cast<AllocationHeader *>(reinterpret_cast<uintptr_t *>(ptr) - sizeof(AllocationHeader));

  // Calculate the start and end addresses of the memory block
  uintptr_t block_start = reinterpret_cast<uintptr_t>(ptr) - header->adjustment;
  uintptr_t block_end = block_start + header->size;

  // Initialize variables to track previous and current FreeBlocks
  FreeBlock *prev_free_block = nullptr;
  FreeBlock *free_block = free_blocks_;

  // Iterate through FreeBlocks to find the appropriate location to deallocate
  while (free_block != nullptr) {
    // Break the loop if the current FreeBlock is beyond the block_end
    if (reinterpret_cast<uintptr_t>(free_block) > block_end) { break; }
    prev_free_block = free_block;
    free_block = free_block->next;
  }

  // Handle different scenarios for deallocation
  if (prev_free_block == nullptr) {
    // If there is no previous FreeBlock, create a new one at the block_start
    prev_free_block = (FreeBlock *) block_start;
    prev_free_block->size = block_end - reinterpret_cast<uintptr_t>(prev_free_block);
    prev_free_block->next = free_blocks_;
    free_blocks_ = prev_free_block;
  } else if (reinterpret_cast<uintptr_t>(prev_free_block) + prev_free_block->size == block_start) {
    // If the previous FreeBlock is adjacent to the block_start, update the size
    prev_free_block->size = header->size;
  } else {
    // Create a new FreeBlock and adjust pointers accordingly
    FreeBlock *t = (FreeBlock *) block_start;
    t->size = header->size;
    t->next = prev_free_block->next;
    prev_free_block->next = t;
    prev_free_block = t;
  }

  // Merge FreeBlocks if needed
  if (free_block != nullptr && reinterpret_cast<uintptr_t>(free_block) == block_end) {
    prev_free_block->size += free_block->size;
    prev_free_block->next = free_block->next;
  }

  // Update the used memory and complete deallocation
  used_memory_ -= header->size;
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
