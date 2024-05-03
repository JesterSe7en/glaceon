#include "AssimpImporter.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "../Logger.h"

namespace glaceon {

void AssimpImporter::ImportObjectModel(const std::string &pFile) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (scene == nullptr) {
    GERROR("Cannot import {} - {}", pFile, importer.GetErrorString());
    return;
  }

  std::vector<float> vertices = GetVertexData(scene, 0);
}

std::vector<float> AssimpImporter::GetVertexData(const aiScene *scene, const int meshIdx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract vertex data");
    return {};
  }

  if (meshIdx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }

  const aiMesh *mesh = scene->mMeshes[meshIdx];
  std::vector<float> verticies;
  verticies.reserve(3 * mesh->mNumVertices);// multiply by 3 cuz (x,y,z)

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    aiVector3d vertex = mesh->mVertices[i];
    verticies.emplace_back(vertex.x);
    verticies.emplace_back(vertex.y);
    verticies.emplace_back(vertex.z);
  }
  return verticies;
}
std::vector<float> AssimpImporter::GetUVData(const aiScene *scene, int meshIdx) { return {}; }
}// namespace glaceon
