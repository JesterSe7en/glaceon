#ifndef GLACEON_GLACEON_TRIANGLEMESH_H_
#define GLACEON_GLACEON_TRIANGLEMESH_H_

#include "VulkanRenderer/VulkanUtils.h"

namespace glaceon {

class TriangleMesh {
 public:
  TriangleMesh(vk::Device logical_device, vk::PhysicalDevice physical_device);
  ~TriangleMesh();

  VulkanUtils::Buffer GetBuffer() const { return buffer_; }

 private:
  vk::Device vk_device_;
  vk::PhysicalDevice vk_physical_device_;
  VulkanUtils::Buffer buffer_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_TRIANGLEMESH_H_
