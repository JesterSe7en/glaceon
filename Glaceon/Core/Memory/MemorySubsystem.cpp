#include "MemorySubsystem.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <memory>

#include "../../Utils.h"
#include "../Logger.h"
#include "LinearAllocator.h"
#include "PoolAllocator.h"

namespace glaceon {

struct memory_stats {
  uint64_t total_allocated;
  uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static std::string tag_names[MEMORY_TAG_MAX_TAGS] = {"Unknown", "Array", "DynArray", "Dict", "String", "Texture"};

static memory_stats stats_ = {0, {0, 0, 0, 0, 0, 0}};

static StackAllocator stack_alloc_ = StackAllocator(1024 * 1024, malloc(1024 * 1024));

MemorySubsystem::MemorySubsystem() {
  // zero out memory stats
  GSetMemory(&stats_, 0, sizeof(stats_));
}

MemorySubsystem::~MemorySubsystem() = default;

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

void *MemorySubsystem::GAllocate(uint64_t num, uint64_t size_of_obj, size_t align, MemoryTag tag) {
  if (num == 0 || size_of_obj == 0) { return nullptr; }
  if (tag == MEMORY_TAG_UNKNOWN) { GWARN("GAllocate called with MEMORY_TAG. ) Specifying a tag is recommended."); }

  switch (tag) {
    case MEMORY_TAG_ARRAY:
      return stack_alloc_.Allocate(num * size_of_obj, align);
    default:
      GWARN("GAllocate called with unknown memory tag.  Specifying a tag is recommended.");
      // do not need to zero as calloc will do that inherently
      return calloc(num, size_of_obj);
  }
}

}// namespace glaceon
