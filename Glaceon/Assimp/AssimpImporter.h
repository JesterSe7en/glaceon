#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

#include <assimp/scene.h>

#include "../Base.h"

namespace glaceon {

struct Assimp_ModelData {
  std::vector<float> vert_data;
  std::vector<float> uv_data;
};

class AssimpImporter {
 public:
  static Assimp_ModelData GLACEON_API ImportObjectModel(const std::string& pFile);

 private:
  static std::vector<float> GetVertexData(const aiScene* scene, size_t meshIdx);
  static std::vector<float> GetUVData(const aiScene* scene, int meshIdx);
};
}// namespace glaceon

#endif//ASSIMPIMPORTER_H
