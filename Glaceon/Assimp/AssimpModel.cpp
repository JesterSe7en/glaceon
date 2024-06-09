#include "AssimpModel.h"
#include "../Core/Memory/MemorySubsystem.h"

namespace glaceon {
void AssimpModel::InitializeVertexData(int num_vertices) {
  void *heap_data = MemorySubsystem::GAllocate(num_vertices, sizeof(glm::vec3), alignof(glm::vec3), MEMORY_TAG_ARRAY);
}
} // glaceon