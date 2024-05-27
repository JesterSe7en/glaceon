#include "AssimpImporter.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "../Core/Logger.h"

namespace glaceon {

Assimp_ModelData AssimpImporter::ImportObjectModel(const std::string& pObjFile) {
  Assimp::Importer importer;
  const aiScene* sceneOBJ = importer.ReadFile(pObjFile, aiProcess_ValidateDataStructure);
  // aiProcess_ValidateDataStructure | aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (sceneOBJ == nullptr) {
    GERROR("Cannot import {} - {}", pObjFile, importer.GetErrorString());
    return Assimp_ModelData{};
  }

  aiString name;
  for (size_t i = 0; i < sceneOBJ->mNumMaterials; i++) {
    aiMaterial* material = sceneOBJ->mMaterials[i];
    // print out all the material proeperties in the .obj file
    if (material->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS) { GINFO("Material name: {}", name.C_Str()); }
    PrintMaterialProperties(material);
  }

  // aiColor3D diffuseColor;
  // aiString name;
  // if (material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) != aiReturn_SUCCESS) {
  //   GERROR("Cannot get diffuse color from material");
  //   return Assimp_ModelData{};
  // }

  glm::vec3 diff;
  diff.r = 0.0f;
  diff.g = 0.0f;
  diff.b = 0.0f;
  // diff.r = diffuseColor.r;
  // diff.g = diffuseColor.g;
  // diff.b = diffuseColor.b;

  return Assimp_ModelData{.vert_data = GetVertexData(sceneOBJ, 0), .diffuse_color = diff};
}

std::vector<glm::vec3> AssimpImporter::GetVertexData(const aiScene* scene, const size_t meshIdx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract vertex data");
    return {};
  }

  if (meshIdx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }

  std::vector<glm::vec3> verticies;
  if (const aiMesh* mesh = scene->mMeshes[meshIdx]; mesh->HasPositions()) {
    verticies.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      aiVector3d vertex = mesh->mVertices[i];
      verticies.emplace_back(vertex.x, vertex.y, vertex.z);
    }
  }
  return verticies;
}

std::vector<glm::vec3> AssimpImporter::GetUVData(const aiScene* scene, int meshIdx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract uv data");
    return {};
  }

  if (meshIdx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }
}

void AssimpImporter::PrintMaterialProperties(const aiMaterial* material) {
  for (unsigned int i = 0; i < material->mNumProperties; ++i) {
    aiMaterialProperty* property = material->mProperties[i];

    GTRACE("Property Name: {}", property->mKey.data);
    GTRACE("Semantic: {}", property->mSemantic);
    GTRACE("Index: {}", property->mIndex);
    GTRACE("Data Length: {}", property->mDataLength);
    GTRACE("Type: {}", property->mType);

    switch (property->mType) {
      case aiPTI_Float: {
        float* data = reinterpret_cast<float*>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(float); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_Double: {
        double* data = reinterpret_cast<double*>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(double); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_String: {
        aiString* data = reinterpret_cast<aiString*>(property->mData);
        GTRACE("Data: {}", data->C_Str());
        break;
      }
      case aiPTI_Integer: {
        int* data = reinterpret_cast<int*>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(int); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_Buffer: {
        unsigned char* data = reinterpret_cast<unsigned char*>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength; ++j) { GTRACE("{}", data[j]); }
        break;
      }
      default:
        GTRACE("Unknown property type.");
        break;
    }

    GTRACE("----------------------------------------");
  }
}
}// namespace glaceon
