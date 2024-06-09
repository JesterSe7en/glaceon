
#ifndef GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_
#define GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_

namespace glaceon {

class AssimpModel {

 public:
  AssimpModel() = default;
  void InitializeVertexData(int num_vertices);

 private:
  glm::vec3* vertices_;

};

} // glaceon

#endif //GLACEON_GLACEON_ASSIMP_ASSIMPMODEL_H_
