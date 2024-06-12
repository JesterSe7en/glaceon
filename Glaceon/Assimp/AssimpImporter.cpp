#include "AssimpImporter.h"

#include <assimp/postprocess.h>

#include <assimp/Importer.hpp>

#include "../Core/Logger.h"
#include "AssimpModel.h"


namespace glaceon {

Assimp_ModelData AssimpImporter::ImportObjectModel(const std::string &obj_file) {
  Assimp::Importer importer;
  const aiScene *scene_obj =
      importer.ReadFile(obj_file,
                        aiProcess_ValidateDataStructure | aiProcess_CalcTangentSpace | aiProcess_Triangulate
                            | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

  if (scene_obj == nullptr) {
    GERROR("Cannot import {} - {}", obj_file, importer.GetErrorString());
    return Assimp_ModelData{};
  }

  ExtractMeshes(scene_obj);

  aiString name;
  for (size_t i = 0; i < scene_obj->mNumMaterials; i++) {
    aiMaterial *material = scene_obj->mMaterials[i];
    // print out all the material properties in the .obj file
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

  return Assimp_ModelData{.vert_data = GetVertexData(scene_obj, 0), .diffuse_color = diff};
}

std::vector<glm::vec3> AssimpImporter::GetVertexData(const aiScene *scene, const size_t mesh_idx) {

  std::vector<glm::vec3> verticies;
  if (const aiMesh *mesh = scene->mMeshes[mesh_idx]; mesh->HasPositions()) {
    verticies.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
      aiVector3d vertex = mesh->mVertices[i];
      verticies.emplace_back(vertex.x, vertex.y, vertex.z);
    }
  }
  return verticies;
}

std::vector<glm::vec3> AssimpImporter::GetUVData(const aiScene *scene, const size_t mesh_idx) {
  if (scene == nullptr) {
    GWARN("No scene provided, cannot extract uv data");
    return {};
  }

  if (mesh_idx > scene->mNumMeshes) {
    GWARN("Cannot find mesh with given mesh index");
    return {};
  }
  return {};
}

void AssimpImporter::PrintMaterialProperties(const aiMaterial *material) {
  for (unsigned int i = 0; i < material->mNumProperties; ++i) {
    aiMaterialProperty *property = material->mProperties[i];

    GTRACE("Property Name: {}", property->mKey.data);
    GTRACE("Semantic: {}", property->mSemantic);
    GTRACE("Index: {}", property->mIndex);
    GTRACE("Data Length: {}", property->mDataLength);
    //    GTRACE("Type: {}", property->mType);

    switch (property->mType) {
      case aiPTI_Float: {
        auto *data = reinterpret_cast<float *>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(float); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_Double: {
        auto *data = reinterpret_cast<double *>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(double); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_String: {
        auto *data = reinterpret_cast<aiString *>(property->mData);
        GTRACE("Data: {}", data->C_Str());
        break;
      }
      case aiPTI_Integer: {
        int *data = reinterpret_cast<int *>(property->mData);
        GTRACE("Data: ");
        for (unsigned int j = 0; j < property->mDataLength / sizeof(int); ++j) { GTRACE("{}", data[j]); }
        break;
      }
      case aiPTI_Buffer: {
        auto *data = reinterpret_cast<unsigned char *>(property->mData);
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
Assimp_MeshData AssimpImporter::ExtractMeshes(const aiScene *scene_obj) {
  if (scene_obj == nullptr) {
    GWARN("No scene provided, cannot extract mesh data");
    return {};
  }

  if (scene_obj->mNumMeshes == 0) {
    GWARN("Scene does not contain any meshes");
    return {};
  }
  GTRACE("number of meshes: {}", scene_obj->mNumMeshes);

  AssimpModel model;

  // Find total vertices across all meshes
  size_t total_vertices = 0;
  for (size_t i = 0; i < scene_obj->mNumMeshes; i++) {
    aiMesh *mesh = scene_obj->mMeshes[i];
    total_vertices += mesh->mNumVertices;
  }

  GTRACE("Total vertices: {}", total_vertices);
  model.InitializeVertexData(total_vertices);

  size_t index = 0;// temporary manage vertex index to maek sure we are adding shit correcly; idiot
  for (size_t i = 0; i < scene_obj->mNumMeshes; i++) {
    aiMesh *mesh = scene_obj->mMeshes[i];
    GTRACE("number of vertices: {}", mesh->mNumVertices);
    for (size_t j = 0; j < mesh->mNumVertices; j++) {
      const glm::vec3 kGlmVert = reinterpret_cast<glm::vec3 *>(mesh->mVertices)[j];
      GTRACE("Assimp Importer: {} {} {}", kGlmVert.x, kGlmVert.y, kGlmVert.z);
      model.AddVertex(kGlmVert);

      // Let's check it
      auto ret = model.GetVertex(index);
      GTRACE("After Adding To Model: {} {} {}", ret.x, ret.y, ret.z);
      index++;
    }
  }

  //   struct Assimp_MeshData {
  //   std::vector<glm::vec3> positions;
  //   std::vector<glm::vec3> normals;
  //   std::vector<glm::vec2> uvs;
  //   std::vector<uint32_t> indices;// indexes of material
  // };

  return {};
}
}// namespace glaceon
