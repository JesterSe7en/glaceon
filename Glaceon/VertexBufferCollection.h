//
// Created by alyxc on 3/25/2024.
//

#ifndef GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_
#define GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_

#include "pch.h"
#include "VulkanRenderer/VulkanUtils.h"

namespace glaceon {

// When we get a bunch of textures and put them on together into a single texture,
// we call that an atlas of textures or we are going to call it just a collection of vertex buffers
class VertexBufferCollection {
 public:
  VertexBufferCollection();
  ~VertexBufferCollection();

  void Add(MeshType type, const std::vector<float>& verticies);

  // Finalizes the collection of vertex buffers, actually allocates the memory
  void Finalize(vk::Device logical_device, vk::PhysicalDevice physical_device);
  VulkanUtils::Buffer vertex_buffer_;

  std::unordered_map<MeshType, int>offsets_;
  std::unordered_map<MeshType, int>sizes_;

 private:
  int offset_;
  vk::Device vk_device_;
  std::vector<float> vertices_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VERTEXBUFFERCOLLECTION_H_
