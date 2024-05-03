#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

#include <assimp/scene.h>

#include "../Base.h"

namespace glaceon {
class AssimpImporter {
 public:
  static void GLACEON_API ImportObjectModel(const std::string& pFile);

 private:
  static std::vector<float> GetVertexData(const aiScene* scene, int meshIdx);
  static std::vector<float> GetUVData(const aiScene* scene, int meshIdx);
};
}// namespace glaceon

#endif//ASSIMPIMPORTER_H
