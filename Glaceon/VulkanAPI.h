//
// Created by alyxc on 3/1/2024.
//

#ifndef GLACEON_GLACEON_VULKANAPI_H_
#define GLACEON_GLACEON_VULKANAPI_H_

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

namespace Glaceon {

class VulkanAPI {

 public:
  static void initVulkan(std::vector<const char *> instance_extensions);

 private:
  static VkInstance vkInstance;
//  static std::shared_ptr<VkInstance> p_vkInstance;

};

} // Glaceon

#endif //GLACEON_GLACEON_VULKANAPI_H_
