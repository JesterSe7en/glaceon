#include "AssimpImporter.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "../Core/Logger.h"

namespace glaceon {

Assimp_ModelData AssimpImporter::ImportObjectModel(const std::string &pObjFile) {
  Assimp::Importer importer;
  const aiScene *sceneOBJ = importer.ReadFile(
      pObjFile,
      aiProcess_ValidateDataStructure | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (sceneOBJ == nullptr) {
    GERROR("Cannot import {} - {}", pObjFile, importer.GetErrorString());
    return Assimp_ModelData{};
  }

  // why is it the second material listed?
  const aiMaterial *material = sceneOBJ->mMaterials[1];

  aiColor3D diffuseColor;
  aiString name;
  if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != aiReturn_SUCCESS) {
    GERROR("Cannot get diffuse color from material");
    return Assimp_ModelData{};
  }

  glm::vec3 diff;
  diff.r = diffuseColor.r;
  diff.g = diffuseColor.g;
  diff.b = diffuseColor.b;

  return Assimp_ModelData{.vert_data = GetVertexData(sceneOBJ, 0), .diffuse_color = diff};
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

std::vector<glm::vec3> AssimpImporter::GetUVData(const aiScene *scene, int meshIdx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract uv data");
    return {};
  }

  if (meshIdx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }
}
}// namespace glaceon
