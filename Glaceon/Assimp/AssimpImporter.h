#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

#include "../Base.h"

struct aiScene;

namespace glaceon {
class AssimpImporter {
 public:
  static bool GLACEON_API ImportObjectModel(const std::string& pFile);
  static bool DoSceneProcessing(const aiScene* scene);
};
}// namespace glaceon

#endif//ASSIMPIMPORTER_H
