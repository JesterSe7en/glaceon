#include "Utils.h"

namespace glaceon {

bool CompareGlmVec3(const glm::vec3& a, const glm::vec3& b) {
  if (a.x == b.x && a.y == b.y && a.z == b.z) { return true; }
  return false;
}

std::vector<size_t> GetIndexFromVertexData(const std::vector<glm::vec3>& vertexData, std::vector<glm::vec3>& uniqueVertex) {
  if (vertexData.empty()) { return {}; }

  std::vector<size_t> indexes;
  for (glm::vec3 vec : vertexData) {
    auto iter = std::ranges::find_if(uniqueVertex, [&](const glm::vec3& v) { return CompareGlmVec3(v, vec); });
    if (iter == uniqueVertex.end()) {
      // New unique vec3 found
      size_t newIndex = uniqueVertex.size();
      uniqueVertex.push_back(vec);
      indexes.push_back(newIndex);
    } else {
      // Existing vec3 found
      indexes.push_back(std::distance(uniqueVertex.begin(), iter));
    }
  }
  return indexes;
}

}// namespace glaceon