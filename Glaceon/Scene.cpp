#include "Scene.h"

namespace glaceon {
Scene::Scene() {

  float x = -0.3f;
  for (float z = -1.0f; z <= 1.0f; z += 0.2f) {
    for (float y = -1.0f; y < 1.0f; y += 0.2f) { triangle_positions_.emplace_back(x, y, z); }
  }

  x = 0.0f;
  for (float z = -1.0f; z <= 1.0f; z += 0.2f) {
    for (float y = -1.0f; y < 1.0f; y += 0.2f) { square_positions_.emplace_back(x, y, z); }
  }

  x = 0.3f;

  for (float z = -1.0f; z <= 1.0f; z += 0.2f) {
    for (float y = -1.0f; y < 1.0f; y += 0.2f) { star_positions_.emplace_back(x, y, z); }
  }
}
Scene::Scene(const Assimp_ModelData& model_data) { vertex_positions = model_data.vert_data; }
}// namespace glaceon
