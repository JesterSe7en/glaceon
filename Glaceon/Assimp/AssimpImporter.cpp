//
// Created by alyxc on 5/2/2024.
//

#include "../Base.h"
#include "../Logger.h"
#include "AssimpImporter.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace glaceon {
bool AssimpImporter::ImportObjectModel(const std::string &pFile) {
  GTRACE("Attempting to import {}", pFile);

  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(pFile,
                                           aiProcess_CalcTangentSpace |
                                           aiProcess_Triangulate |
                                           aiProcess_JoinIdenticalVertices |
                                           aiProcess_SortByPType);
  if (scene == nullptr) {
    GERROR("Cannot import {}", pFile);
    return false;
  }

  DoSceneProcessing(scene);

  return true;
}

bool AssimpImporter::DoSceneProcessing(const aiScene *scene) { return true; }
}// glaceon