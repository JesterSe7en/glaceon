//
// Created by alyxc on 3/25/2024.
//

#include "SquareMesh.h"

namespace glaceon {
SquareMesh::SquareMesh(vk::Device logical_device, vk::PhysicalDevice physical_device)
    : vk_device_(logical_device), vk_physical_device_(physical_device) {

  std::vector<float> vertices = {{-0.05f, 0.05f,  1.0f, 0.0f, 0.0f, -0.05f, -0.05f, 1.0f, 0.0f, 0.0f,
                                  0.05f,  -0.05f, 1.0f, 0.0f, 0.0f, 0.05f,  -0.05f, 1.0f, 0.0f, 0.0f,
                                  0.05f,  0.05f,  1.0f, 0.0f, 0.0f, -0.05f, 0.05f,  1.0f, 0.0f, 0.0f}};

  VulkanUtils::BufferInputParams params;
  params = {vk_device_, vk_physical_device_, vertices.size() * sizeof(float), vk::BufferUsageFlagBits::eVertexBuffer};

  buffer_ = VulkanUtils::CreateBuffer(params);

  // map a memory location to the buffer
  void *memory_loc = vk_device_.mapMemory(buffer_.buffer_memory, 0, VK_WHOLE_SIZE, {});
  memcpy(memory_loc, vertices.data(), vertices.size() * sizeof(float));
  vk_device_.unmapMemory(buffer_.buffer_memory);
}
SquareMesh::~SquareMesh() {
  vk_device_.destroyBuffer(buffer_.buffer);
  vk_device_.freeMemory(buffer_.buffer_memory);
  buffer_.buffer = VK_NULL_HANDLE;
  buffer_.buffer_memory = VK_NULL_HANDLE;
}
}// namespace glaceon