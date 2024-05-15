
#include "LinearAllocator.h"

#include "../Utils.h"

namespace glaceon {
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

void LinearAllocator::Deallocate(void *ptr) { assert(false && "Deallocate not implemented for LinearAllocator; use clear() instead"); }

/**
 * @brief Clears the LinearAllocator, resetting the current position to the start and setting the used memory to 0.
 *
 * This function is used to reset the LinearAllocator to its initial state. It is typically called when you want to reuse the LinearAllocator for a new allocation.
 */
void LinearAllocator::Clear() {
  current_pos_ = start_;
  used_memory_ = 0;
}

}// namespace glaceon