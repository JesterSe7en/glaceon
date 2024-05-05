#ifndef GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_
#define GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_

#include <cstdint>

#include "VulkanRenderer/VulkanUtils.h"
#include "pch.h"

namespace glaceon {

// When we get a bunch of textures and put them on together into a single texture,
// we call that an atlas of textures or we are going to call it just a collection of vertex buffers
class VertexBufferCollection {
 public:
  VertexBufferCollection();
  ~VertexBufferCollection();

  void Add(MeshType type, const std::vector<float> &verticies, const std::vector<uint32_t> &indexes);
  void Add(const std::vector<glm::vec3> &verticies, const std::vector<uint32_t> &indexes);

  // Finalizes the collection of vertex buffers, actually allocates the memory
  void Finalize(vk::Device logical_device, vk::PhysicalDevice physical_device, vk::Queue queue, vk::CommandBuffer command_buffer);

  VulkanUtils::Buffer vertex_buffer_;
  VulkanUtils::Buffer index_buffer_;

  std::unordered_map<MeshType, int> first_indexes_;
  std::unordered_map<MeshType, int> index_counts_;

 private:
  int offset_;
  vk::Device vk_device_;
  std::vector<float> vertices_;
  std::vector<uint32_t> indexes_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_
