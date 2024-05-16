#include "FreeListAllocator.h"

#include "../Utils.h"

namespace glaceon {

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
  FreeBlock *prev_free_block = nullptr;

  // Start at the beginning of the free list
  FreeBlock *free_block = free_blocks_;

  /*
   * Iterate through the free list, until we find a free block
   * that is big enough to hold the requested allocation.
   */
  while (free_block != nullptr) {

    // Calculate the adjustment needed for alignment
    uint8_t adjustment = AlignSize(free_block, alignment);

    // Calculate the total size needed for the requested allocation
    size_t total_size = size + adjustment;

    // Check if the free block is big enough
    if (free_block->size <= total_size) {
      // The free block is not big enough, so continue to the next one
      prev_free_block = free_block;
      free_block = free_block->next;
      continue;
    }

    // Now check if there is enough room for the AllocationHeader
    if (free_block->size - total_size >= sizeof(AllocationHeader)) {
      // There is enough room for the AllocationHeader

      /*
       * Use the free block as the allocated block by setting the requested
       * allocation size to the size of the free block instead of creating
       * a new FreeBlock.
       */
      total_size = free_block->size;

      /*
       * Remove the free block from the free list by updating the next
       * pointer of the previous free block (or the free list if the current
       * block is the first in the list).
       */
      if (prev_free_block != nullptr) {
        prev_free_block->next = free_block->next;
      } else {
        // the current block is the first in the list
        free_blocks_ = free_block->next;
      }
    } else {
      // There is not enough room for the AllocationHeader, so create a new FreeBlock

      /*
       * Create a new FreeBlock after the current one, and point the next
       * pointer of the current FreeBlock to the new FreeBlock.
       */
      auto *next_block = reinterpret_cast<FreeBlock *>(reinterpret_cast<uintptr_t>(free_block) + total_size);
      next_block->size = free_block->size - total_size;
      next_block->next = free_block->next;

      /*
       * Update the next pointer of the previous free block (or the free
       * list if the current block is the first in the list).
       */
      if (prev_free_block != nullptr) {
        prev_free_block->next = next_block;
      } else {
        free_blocks_ = next_block;
      }
    }

    // Calculate the aligned address of the allocated block
    uintptr_t aligned_address = reinterpret_cast<uintptr_t>(free_block) + adjustment;

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

}// namespace glaceon