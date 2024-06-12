#ifndef GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_
#define GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_

namespace glaceon {

class AssimpModel {

 public:
  AssimpModel() = default;
  void InitializeVertexData(size_t num_vertices);
  void AddVertex(glm::vec3 vertex);
  glm::vec3 GetVertex(size_t index);

  glm::vec3* GetVertices() { return vertices_.get(); }

 private:
  std::unique_ptr<glm::vec3[]> vertices_;
  uint64_t vertices_offset_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_
