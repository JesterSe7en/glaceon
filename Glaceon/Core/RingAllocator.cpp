#include "RingAllocator.h"

#include "../Utils.h"

namespace glaceon {

RingAllocator::RingAllocator(size_t size, void *start) : head_(0), tail_(0), is_full_(false) {
  size_ = size;
  start_ = start;
  num_allocations_ = 0;
  used_memory_ = 0;
}

RingAllocator::~RingAllocator() {
  start_ = nullptr;
  num_allocations_ = 0;
  used_memory_ = 0;
}

/**
 * @brief Allocates a block of memory from the RingAllocator.
 *
 * @param size The size of the memory block to allocate.
 * @param alignment The alignment of the memory block.
 * @return void* A pointer to the allocated memory block, or nullptr if the allocation failed.
 *
 * This function tries to allocate a block of memory from the RingAllocator. It first checks if the size and
 * alignment are valid. Then it calculates the aligned address of the start of the memory block. 
 *
 * Next, it checks if there is enough space in the RingAllocator to allocate the requested memory block. It does this
 * by checking if the RingAllocator is not full and if the size of the memory block is less than or equal to the 
 * available space at the aligned address.
 *
 * If there is enough space, it updates the head pointer of the RingAllocator to the aligned address plus the size of
 * the memory block. If the updated head pointer exceeds the size of the RingAllocator, it wraps around to the start.
 * If the updated head pointer is equal to the tail pointer, it marks the RingAllocator as full.
 *
 * Finally, it returns the aligned address as the pointer to the allocated memory block.
 */
void *RingAllocator::Allocate(size_t size, uint8_t alignment) {
  // Check if the size and alignment are valid
  if (size == 0 || alignment == 0) { return nullptr; }

  // Calculate the aligned address of the start of the memory block
  void *aligned_address = AlignAddress(start_, alignment);

  // Check if there is enough space in the RingAllocator to allocate the requested memory block
  bool enough_space = !is_full_ && size <= GetAvailableSpace(reinterpret_cast<uintptr_t>(aligned_address));
  if (!enough_space) { return nullptr; }

  // Update the head pointer of the RingAllocator to the aligned address plus the size of the memory block
  head_ = reinterpret_cast<uintptr_t>(aligned_address) + size;
  if (head_ >= size_) { head_ -= size_; } // Wrap around to the start if the updated head pointer exceeds the size of the RingAllocator
  if (head_ == tail_) { is_full_ = true; }// Mark the RingAllocator as full if the updated head pointer is equal to the tail pointer

  return aligned_address;
}

/**
 * @brief Deallocates a block of memory from the RingAllocator.
 *
 * @param ptr A pointer to the memory block to deallocate.
 *
 * This function checks if the given pointer is null. If it is, the function
 * returns immediately. Otherwise, it checks if the pointer matches the tail
 * pointer of the RingAllocator. If it does, the function updates the tail
 * pointer and marks the RingAllocator as not full.
 *
 * The function uses reinterpret_cast to convert the void pointer to an
 * unsigned integer pointer (uintptr_t) and then compares it with the tail
 * pointer of the RingAllocator. If they match, the function increments the tail
 * pointer by the size of the RingAllocator and wraps it around to the start if
 * it exceeds the size. Finally, it marks the RingAllocator as not full.
 */
void RingAllocator::Deallocate(void *ptr) {
  // Check if the pointer is null
  if (ptr == nullptr) { return; }

  // Check if the pointer matches the tail pointer of the RingAllocator
  if (reinterpret_cast<uintptr_t>(ptr) == tail_) {
    // Increment the tail pointer by the size of the RingAllocator
    tail_ += size_;
    // Wrap the tail pointer around to the start if it exceeds the size
    if (tail_ >= size_) { tail_ = 0; }
    // Mark the RingAllocator as not full
    is_full_ = false;
  }
}

/**
 * @brief Calculates the amount of free space available at the given address.
 *
 * This function calculates the amount of free space available at the given address. It takes into account the case
 * where the head and tail pointers have wrapped around the ring buffer.
 *
 * @param address The address to check for free space.
 * @return The amount of free space available at the given address.
 */
uintptr_t RingAllocator::GetAvailableSpace(uintptr_t address) {
  // If the head pointer has wrapped around the ring buffer, we need to consider both the distance from the head
  // pointer to the given address, and the distance from the tail pointer to the given address.
  if (head_ >= tail_) {
    // If the given address is greater than or equal to the head pointer, we need to subtract the head pointer from
    // the given address to get the distance.
    if (address >= head_) {
      // Subtract the head pointer from the given address to get the distance.
      // The result is the amount of free space available at the given address.
      return size_ - address + tail_;
    }
    // If the given address is less than the head pointer, we need to subtract the tail pointer from the given address
    // to get the distance.
    // The result is the amount of free space available at the given address.
    return tail_ - address;
  }
  // If the head pointer has not wrapped around the ring buffer, we only need to consider the distance from the
  // tail pointer to the given address.
  // The result is the amount of free space available at the given address.
  return tail_ - address;
}

}// namespace glaceon