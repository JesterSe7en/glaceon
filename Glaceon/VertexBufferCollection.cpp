#include "VertexBufferCollection.h"

#include "Base.h"
#include "Logger.h"
#include "VulkanRenderer/VulkanUtils.h"

namespace glaceon {
VertexBufferCollection::VertexBufferCollection() : offset_(0) {}
VertexBufferCollection::~VertexBufferCollection() {
  VK_ASSERT(vk_device_ != VK_NULL_HANDLE, "Logical device is null");

  vk_device_.destroyBuffer(vertex_buffer_.buffer);
  vk_device_.freeMemory(vertex_buffer_.buffer_memory);
  vertex_buffer_.buffer = VK_NULL_HANDLE;
  vertex_buffer_.buffer_memory = VK_NULL_HANDLE;

  vk_device_.destroyBuffer(index_buffer_.buffer);
  vk_device_.freeMemory(index_buffer_.buffer_memory);
  index_buffer_.buffer = VK_NULL_HANDLE;
  index_buffer_.buffer_memory = VK_NULL_HANDLE;
}

void VertexBufferCollection::Add(MeshType type, const std::vector<float> &verticies, const std::vector<uint32_t> &indexes) {
  // divide by 7 to get the number of vertices, since vectors will be structured as (x, y, r, g, b, u, v)
  int vertex_count = static_cast<int>(verticies.size() / 7);
  int index_count = static_cast<int>(indexes.size());// Total number of indexes to draw
  int last_index =
      static_cast<int>(indexes_.size());// we want to append the new indexes vector to the old one, so grab the end of the indexes_ vector

  first_indexes_.insert(std::make_pair(type, last_index));
  index_counts_.insert(std::make_pair(type, index_count));
  for (float v : verticies) { vertices_.push_back(v); }
  for (uint32_t i : indexes) { indexes_.push_back(i + offset_); }
  offset_ += vertex_count;
}

void VertexBufferCollection::Add(const std::vector<glm::vec3> &verticies, const std::vector<uint32_t> &indexes) {
  int vertex_count = static_cast<int>(verticies.size());
  int index_count = static_cast<int>(indexes.size());
  int last_index = static_cast<int>(indexes_.size());

  first_indexes_.insert(std::make_pair(MeshType::kVertex, last_index));
  index_counts_.insert(std::make_pair(MeshType::kVertex, index_count));
  for (glm::vec3 v : verticies) {
    vertices_.push_back(v.x);
    vertices_.push_back(v.y);
    vertices_.push_back(v.z);
  }
  for (uint32_t i : indexes) { indexes_.push_back(i + offset_); }
  offset_ += vertex_count;
}

void VertexBufferCollection::Finalize(vk::Device logical_device, vk::PhysicalDevice physical_device, vk::Queue queue,
                                      vk::CommandBuffer command_buffer) {
  vk_device_ = logical_device;

  if (vertices_.empty()) {
    GERROR("Cannot finialize vertex buffer collection as no verticies were given.");
    return;
  }

  // ----------- Vertex Buffer Transfer ------------
  VulkanUtils::BufferInputParams params = {logical_device, physical_device, vertices_.size() * sizeof(float), vk::BufferUsageFlagBits::eTransferSrc,
                                           vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};

  VulkanUtils::Buffer staging_buffer = VulkanUtils::CreateBuffer(params);

  // map a memory location to the buffer
  void *memory_loc = logical_device.mapMemory(staging_buffer.buffer_memory, 0, VK_WHOLE_SIZE, {});
  memcpy(memory_loc, vertices_.data(), vertices_.size() * sizeof(float));
  logical_device.unmapMemory(staging_buffer.buffer_memory);

  // copy from staging buffer (host local memory) to vertex buffer (device local memory)
  params.memory_property_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
  params.buffer_usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;
  vertex_buffer_ = VulkanUtils::CreateBuffer(params);

  VulkanUtils::CopyBuffer(staging_buffer, vertex_buffer_, vertices_.size() * sizeof(float), queue, command_buffer);

  logical_device.destroyBuffer(staging_buffer.buffer);
  logical_device.freeMemory(staging_buffer.buffer_memory);

  // ----------- Index Buffer Transfer ------------
  params = {logical_device, physical_device, indexes_.size() * sizeof(uint32_t), vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent};

  staging_buffer = VulkanUtils::CreateBuffer(params);

  memory_loc = logical_device.mapMemory(staging_buffer.buffer_memory, 0, VK_WHOLE_SIZE, {});
  memcpy(memory_loc, indexes_.data(), indexes_.size() * sizeof(uint32_t));
  logical_device.unmapMemory(staging_buffer.buffer_memory);

  // copy from staging buffer to index buffer
  params.memory_property_flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
  params.buffer_usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;
  index_buffer_ = VulkanUtils::CreateBuffer(params);

  VulkanUtils::CopyBuffer(staging_buffer, index_buffer_, indexes_.size(), queue, command_buffer);

  logical_device.destroyBuffer(staging_buffer.buffer);
  logical_device.freeMemory(staging_buffer.buffer_memory);
}

}// namespace glaceon
