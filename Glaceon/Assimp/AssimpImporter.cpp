#include "AssimpImporter.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "../Logger.h"

namespace glaceon {

Assimp_ModelData AssimpImporter::ImportObjectModel(const std::string &pFile) {
  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (scene == nullptr) {
    GERROR("Cannot import {} - {}", pFile, importer.GetErrorString());
    return Assimp_ModelData{};
  }

  return Assimp_ModelData{.vert_data = GetVertexData(scene, 0), .uv_data = GetUVData(scene, 0)};
}

std::vector<glm::vec3> AssimpImporter::GetVertexData(const aiScene *scene, const size_t meshIdx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract vertex data");
    return {};
  }

  if (meshIdx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }

  std::vector<glm::vec3> verticies;
  if (const aiMesh *mesh = scene->mMeshes[meshIdx]; mesh->HasPositions()) {
    verticies.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      aiVector3d vertex = mesh->mVertices[i];
      verticies.emplace_back(vertex.x, vertex.y, vertex.z);
    }
  }
  return verticies;
}

std::vector<glm::vec3> AssimpImporter::GetUVData(const aiScene *scene, int meshIdx) { return {}; }
}// namespace glaceon
