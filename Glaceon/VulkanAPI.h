#ifndef GLACEON_GLACEON_VULKANAPI_H_
#define GLACEON_GLACEON_VULKANAPI_H_

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Glaceon {

class VulkanAPI {
 public:
  static void initVulkan(std::vector<const char *> instance_extensions);
  static VkInstance getVulkanInstance() { return vkInstance; }

 private:
  static VkInstance vkInstance;
  static VkDevice vkDevice;
  static VkQueue vkQueue;
  static VkDescriptorPool vkDescriptorPool;
  //  static std::shared_ptr<VkInstance> p_vkInstance;
};

}  // namespace Glaceon

#endif  // GLACEON_GLACEON_VULKANAPI_H_
