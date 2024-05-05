#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

#include <assimp/scene.h>

#include "../Base.h"

namespace glaceon {

struct Assimp_ModelData {
  std::vector<glm::vec3> vert_data;
  std::vector<glm::vec3> uv_data;
};

class AssimpImporter {
 public:
  static Assimp_ModelData GLACEON_API ImportObjectModel(const std::string& pFile);

 private:
  static std::vector<glm::vec3> GetVertexData(const aiScene* scene, size_t meshIdx);
  static std::vector<glm::vec3> GetUVData(const aiScene* scene, int meshIdx);
};
}// namespace glaceon

#endif//ASSIMPIMPORTER_H
