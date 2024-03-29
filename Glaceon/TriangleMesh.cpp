#include "TriangleMesh.h"

namespace glaceon {
TriangleMesh::TriangleMesh(vk::Device logical_device, vk::PhysicalDevice physical_device)
    : vk_device_(logical_device), vk_physical_device_(physical_device) {
  // layout is vec2 (x,y) position, vec3 (r,g,b) color
  // This is defining a small blue triangle
  std::vector<float> vertices = {0.0f, -0.05f, 0.0f,   0.0f,  1.0f, 0.05f, 0.05f, 0.0f,
                                 0.0f, 1.0f,   -0.05f, 0.05f, 0.0f, 0.0f,  1.0f};

  VulkanUtils::BufferInputParams params;
  params = {vk_device_, vk_physical_device_, vertices.size() * sizeof(float), vk::BufferUsageFlagBits::eVertexBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};

  buffer_ = VulkanUtils::CreateBuffer(params);

  // map a memory location to the buffer
  void *memory_loc = vk_device_.mapMemory(buffer_.buffer_memory, 0, VK_WHOLE_SIZE, {});
  memcpy(memory_loc, vertices.data(), vertices.size() * sizeof(float));
  vk_device_.unmapMemory(buffer_.buffer_memory);
}

TriangleMesh::~TriangleMesh() {
  vk_device_.destroyBuffer(buffer_.buffer);
  vk_device_.freeMemory(buffer_.buffer_memory);
  buffer_.buffer = VK_NULL_HANDLE;
  buffer_.buffer_memory = VK_NULL_HANDLE;
}
}// namespace glaceon