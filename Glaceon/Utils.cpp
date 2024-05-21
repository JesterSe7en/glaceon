#include "Utils.h"

#include <cstdint>

namespace glaceon {

bool CompareGlmVec3(const glm::vec3& a, const glm::vec3& b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z) { return true; }
  return false;
}

std::vector<uint32_t> GetIndexFromVertexData(const std::vector<glm::vec3>& vertexData, std::vector<glm::vec3>& uniqueVertex) {
  if (vertexData.empty()) { return {}; }

  std::vector<uint32_t> indexes;
  for (glm::vec3 vec : vertexData) {
    auto iter = std::ranges::find_if(uniqueVertex, [&](const glm::vec3& v) { return CompareGlmVec3(v, vec); });
    if (iter == uniqueVertex.end()) {
      // New unique vec3 found
      uint32_t newIndex = uniqueVertex.size();
      uniqueVertex.push_back(vec);
      indexes.push_back(newIndex);
    } else {
      // Existing vec3 found
      indexes.push_back(std::distance(uniqueVertex.begin(), iter));
    }
  }
  return indexes;
}

void* AlignAddress(void* address, uint32_t alignment) {
  const size_t kM = alignment - 1;
  // cast to uintptr_t so that the + operation works; doing pointer arithmetic on a void* is undefined behavior
  return (void*) ((reinterpret_cast<uintptr_t>(address) + kM) & ~kM);
}

uint8_t AlignSize(const void* address, uint8_t alignment) {
  auto addr = reinterpret_cast<uintptr_t>(address);
  uint8_t adjustment = alignment - (addr & alignment - 1);

  if (adjustment == alignment) { return 0; }
  return adjustment;
}

uint8_t AlignSizeWithHeader(const void* address, uint8_t alignment, size_t headerSize) {
  uint8_t adjustment = AlignSize(address, alignment);
  auto addr = reinterpret_cast<uintptr_t>(address);

  if (adjustment < headerSize) {
    uint8_t headerAdjustment = headerSize - adjustment;
    uintptr_t rounded = addr + alignment - 1 - (addr + alignment - 1) % alignment;

    if (headerAdjustment > rounded - addr) {
      adjustment += static_cast<uint8_t>(rounded - addr);
    } else {
      adjustment += headerAdjustment;
    }
  }

  return adjustment;
}

}// namespace glaceon
