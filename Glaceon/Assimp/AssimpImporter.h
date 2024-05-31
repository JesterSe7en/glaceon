#ifndef ASSIMPIMPORTER_H
#define ASSIMPIMPORTER_H

#include <assimp/scene.h>

#include "../Core/Base.h"

namespace glaceon {

struct Assimp_ModelData {
  std::vector<glm::vec3> vert_data;
  glm::vec3 diffuse_color;
  // std::vector<glm::vec3> uv_data;
};

struct Assimp_MeshData {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
  std::vector<uint32_t> indices;// indexes of material
};

class AssimpImporter {
 public:
  static Assimp_ModelData GLACEON_API ImportObjectModel(const std::string& obj_file);

 private:
  static void PrintMaterialProperties(const aiMaterial* material);
  static std::vector<glm::vec3> GetVertexData(const aiScene* scene, const size_t mesh_idx);
  static std::vector<glm::vec3> GetUVData(const aiScene* scene, const size_t mesh_idx);

  static Assimp_MeshData ExtractMeshes(const aiScene* scene_obj);
};
}// namespace glaceon

#endif//ASSIMPIMPORTER_H
