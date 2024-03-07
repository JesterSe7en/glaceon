#include "VulkanContext.h"

#include <algorithm>

namespace Glaceon {

VulkanContext::VulkanContext()
    : backend(*this), device(*this), swapChain(*this), deviceExtensions(), instanceExtensions() {}
void VulkanContext::AddDeviceExtension(const char *ext) { deviceExtensions.push_back(ext); }
void VulkanContext::AddInstanceExtension(const char *ext) { instanceExtensions.push_back(ext); }
void VulkanContext::RemoveDeviceExtension(const char *ext) {
  auto it = std::find(deviceExtensions.begin(), deviceExtensions.end(), ext);
  if (it != deviceExtensions.end()) {
    deviceExtensions.erase(it);
  }
}
void VulkanContext::RemoveInstanceExtension(const char *ext) {
  auto it = std::find(instanceExtensions.begin(), instanceExtensions.end(), ext);
  if (it != instanceExtensions.end()) {
    instanceExtensions.erase(it);
  }
}

}  // namespace Glaceon
