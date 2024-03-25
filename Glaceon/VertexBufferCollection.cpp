//
// Created by alyxc on 3/25/2024.
//

#include "VertexBufferCollection.h"
#include "Base.h"
#include "Logger.h"

namespace glaceon {
VertexBufferCollection::VertexBufferCollection() : offset_(0) {}
VertexBufferCollection::~VertexBufferCollection() {
  VK_ASSERT(vk_device_ != VK_NULL_HANDLE, "Logical device is null");

  vk_device_.destroyBuffer(vertex_buffer_.buffer);
  vk_device_.freeMemory(vertex_buffer_.buffer_memory);
  vertex_buffer_.buffer = VK_NULL_HANDLE;
  vertex_buffer_.buffer_memory = VK_NULL_HANDLE;
}
void VertexBufferCollection::Add(MeshType type, const std::vector<float> &vector) {
  for (float v : vector) { vertices_.push_back(v); }
  // divide by 5 to get the number of vertices, since vectors will be structured as (x, y, r, g, b)
  int vertex_count = static_cast<int>(vector.size() / 5);

  offsets_.insert(std::make_pair(type, offset_));
  sizes_.insert(std::make_pair(type, vertex_count));
  offset_ += vertex_count;
}
void VertexBufferCollection::Finalize(vk::Device logical_device, vk::PhysicalDevice physical_device) {
  vk_device_ = logical_device;
  VulkanUtils::BufferInputParams params;
  params = {logical_device, physical_device, vertices_.size() * sizeof(float), vk::BufferUsageFlagBits::eVertexBuffer};

  vertex_buffer_ = VulkanUtils::CreateBuffer(params);

  // map a memory location to the buffer
  void *memory_loc = logical_device.mapMemory(vertex_buffer_.buffer_memory, 0, VK_WHOLE_SIZE, {});
  memcpy(memory_loc, vertices_.data(), vertices_.size() * sizeof(float));
  logical_device.unmapMemory(vertex_buffer_.buffer_memory);
}

}// namespace glaceon