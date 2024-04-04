#include "Scene.h"

namespace glaceon {
Scene::Scene() {

  float x = -0.3f;
  for (float y = -1.0f; y < 1.0f; y += 0.2f) { triangle_positions_.push_back(glm::vec3(x, y, 0.0f)); }

  x = 0.0f;
  for (float y = -1.0f; y < 1.0f; y += 0.2f) { square_positions_.push_back(glm::vec3(x, y, 0.0f)); }

  x = 0.3f;
  for (float y = -1.0f; y < 1.0f; y += 0.2f) { star_positions_.push_back(glm::vec3(x, y, 0.0f)); }
}
}// namespace glaceon