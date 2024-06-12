#include "AssimpModel.h"

#include "../Core/Memory/MemorySubsystem.h"

namespace glaceon {

void AssimpModel::InitializeVertexData(size_t num_vertices) {
  // allocate memory
  vertices_.reset(static_cast<glm::vec3 *>(
      MemorySubsystem::GAllocate(num_vertices, sizeof(glm::vec3), alignof(glm::vec3), MEMORY_TAG_ARRAY)));
  vertices_offset_ = 0;
}

void AssimpModel::AddVertex(glm::vec3 vertex) {
  vertices_[vertices_offset_] = vertex;
  vertices_offset_++;
  //  vertices_offset_ += sizeof(glm::vec3);
}

glm::vec3 AssimpModel::GetVertex(size_t index) { return vertices_[index]; }

}// namespace glaceon