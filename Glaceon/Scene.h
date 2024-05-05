#ifndef GLACEON_GLACEON_SCENE_H_
#define GLACEON_GLACEON_SCENE_H_
#include "Assimp/AssimpImporter.h"

namespace glaceon {

// This defines all the triangles(TriangleMesh) in the scene
class Scene {
 public:
  Scene();
  explicit Scene(const Assimp_ModelData &model_data);

  std::vector<glm::vec3> triangle_positions_;
  std::vector<glm::vec3> square_positions_;
  std::vector<glm::vec3> star_positions_;

  std::vector<float> vertex_positions;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_SCENE_H_
