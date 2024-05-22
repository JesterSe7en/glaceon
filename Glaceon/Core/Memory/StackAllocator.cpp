#include "StackAllocator.h"

#include "../../Utils.h"

namespace glaceon {

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

}// namespace glaceon