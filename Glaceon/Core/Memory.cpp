#include "Memory.h"
#include "Logger.h"

#define WIN32_LEAN_AND_MEAN
#include <string.h>
#include <windows.h>

namespace glaceon {

struct memory_stats {
  uint64_t total_allocated;
  uint64_t tagged_allocations[MEMORY_TAG_MAX_TAGS];
};

static std::string tag_names[MEMORY_TAG_MAX_TAGS] = {
    "Unknown",
    "Array",
    "DynArray",
    "Dict",
    "String",
    "Texture"};

static memory_stats stats_ = {0, {0, 0, 0, 0, 0, 0}};

MemorySubsystem::MemorySubsystem() {
  // zero out memory stats
  GSetMemory(&stats_, 0, sizeof(stats_));
}

MemorySubsystem::~MemorySubsystem() {}

void *MemorySubsystem::GAllocate(uint64_t size, MemoryTag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) {
    GWARN("GAllocate called with MEMORY_TAG_UNKNOWN.  Specifying a tag is recommended.");
  }

  // update stats
  stats_.total_allocated += size;
  GTRACE("Total allocated: {}", stats_.total_allocated);
  stats_.tagged_allocations[tag] += size;
  GTRACE("Tagged allocations: {}", tag_names[tag]);

  // TODO: allow for alignment bool?
  return (void *) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);// also zeroes memory
}

void MemorySubsystem::GFree(void *mem_block, uint64_t size, MemoryTag tag) {
  if (tag == MEMORY_TAG_UNKNOWN) {
    GWARN("GFree called with MEMORY_TAG_UNKNOWN.  Specifying a tag is recommended.");
  }

  // update stats
  stats_.total_allocated -= size;
  stats_.tagged_allocations[tag] -= size;

  // TODO: allow for alignment bool?
  HeapFree(GetProcessHeap(), 0, mem_block);
}

// Following functions are just wrappers around standard C functions

void *MemorySubsystem::GZeroMemory(void *mem_block, uint64_t size) {
  return memset(mem_block, 0, size);
}

void *MemorySubsystem::GCopyMemory(void *dest, const void *src, uint64_t size) {
  return memcpy(dest, src, size);
}

void *MemorySubsystem::GSetMemory(void *dest, int value, uint64_t size) {
  return memset(dest, value, size);
}

std::string MemorySubsystem::GetStats() {
  std::string buffer = "System memory in use:\n";

  // print out memory stats for each memory tag type
  const uint64_t kKib = 1024;
  const uint64_t kMib = kKib * 1024;
  const uint64_t kGib = kMib * 1024;

  for (uint32_t i = 0; i < MEMORY_TAG_MAX_TAGS; i++) {
    if (stats_.tagged_allocations[i] >= kGib) {
      buffer.append(std::sprintf("%s: %.2f %sd\n", tag_names[i].c_str(), stats_.tagged_allocations[i] / static_cast<float>(kGib), "GiB"));
    } else if (stats_.tagged_allocations[i] >= kMib) {
      buffer += std::snprintf("%s: %.2f %sd\n", tag_names[i].c_str(), stats_.tagged_allocations[i] / static_cast<float>(kMib), "MiB");
    } else if (stats_.tagged_allocations[i] >= kKib) {
      buffer += std::snprintf("%s: %.2f %sd\n", tag_names[i].c_str(), stats_.tagged_allocations[i] / static_cast<float>(kKib), "KiB");
    } else {
      buffer += std::snprintf("%s: %.2f %sd\n", tag_names[i].c_str(), stats_.tagged_allocations[i], "B");
    }
    buffer += "\n";// Add a new line for each memory tag type
  }

  return buffer;
}
}
}// namespace glaceon
