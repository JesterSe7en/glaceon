#ifndef GLACEON_GLACEON_SCENE_H_
#define GLACEON_GLACEON_SCENE_H_

namespace glaceon {

class Scene {
 public:
  Scene();

  std::vector<glm::vec3> triangle_positions_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_SCENE_H_