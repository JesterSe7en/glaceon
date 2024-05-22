#include "PoolAllocator.h"

#include "../../Utils.h"

namespace glaceon {

/**
 * @brief Constructor for PoolAllocator.
 *
 * @param obj_size Size of each object in the pool.
 * @param alignment Alignment requirement for each object.
 * @param size Total size of the memory pool.
 * @param start Starting address of the memory pool.
 */
PoolAllocator::PoolAllocator(size_t obj_size, uint8_t alignment, size_t size, void *start) : objectAlignment(alignment), objectSize(obj_size) {
  // Calculate adjustment for alignment
  uint8_t adjustment = AlignSize(start, alignment);

  // Set the free list pointer
  free_list_ = reinterpret_cast<void **>(reinterpret_cast<uintptr_t>(start) + adjustment);

  // Calculate the number of objects that can fit in the pool
  size_t numObjects = (size - adjustment) / obj_size;

  // Initialize the free list
  void **p = free_list_;
  for (size_t i = 0; i < numObjects - 1; i++) {
    *p = reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(start) + adjustment + i * obj_size);
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