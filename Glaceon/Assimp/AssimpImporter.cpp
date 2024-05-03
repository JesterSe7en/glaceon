#include "AssimpImporter.h"

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>

#include "../Logger.h"

namespace glaceon {
bool AssimpImporter::ImportObjectModel(const std::string &pFile) {
  GTRACE("Attempting to import {}", pFile);

  Assimp::Importer importer;
  const aiScene *scene =
      importer.ReadFile(pFile, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);
  if (scene == nullptr) {
    GERROR("Error importing object model {} - {}", pFile, importer.GetErrorString());
    return false;
  }

  DoSceneProcessing(scene);

  return true;
}

bool AssimpImporter::DoSceneProcessing(const aiScene *scene) {
  if (scene->mNumMeshes <= 0) return false;

  const aiMesh *mesh = scene->mMeshes[0];

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    aiVector3d vertex = mesh->mVertices[i];
    GTRACE("Vertex {}: ({}, {}, {})", i, vertex.x, vertex.y, vertex.z);
  }
  return true;
}
}// namespace glaceon