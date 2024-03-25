//
// Created by alyxc on 3/25/2024.
//

#ifndef GLACEON_GLACEON_SQUAREMESH_H_
#define GLACEON_GLACEON_SQUAREMESH_H_

#include "VulkanRenderer/VulkanUtils.h"

namespace glaceon {

class SquareMesh {
 public:
  SquareMesh(vk::Device logical_device, vk::PhysicalDevice physical_device);
  ~SquareMesh();

  [[nodiscard]] VulkanUtils::Buffer GetBuffer() const { return buffer_; }

 private:
  vk::Device vk_device_;
  vk::PhysicalDevice vk_physical_device_;
  VulkanUtils::Buffer buffer_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_SQUAREMESH_H_
