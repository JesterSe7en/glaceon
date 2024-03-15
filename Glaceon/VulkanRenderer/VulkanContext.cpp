#include "VulkanContext.h"

namespace Glaceon {

VulkanContext::VulkanContext()
    : backend(*this),
      device(*this),
      swapChain(*this),
      renderPass(*this),
      pipeline(*this),
      commandPool(*this),
      sync(*this),
      deviceExtensions(),
      instanceExtensions() {}
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

void VulkanContext::Destroy() {
  sync.Destroy();
  commandPool.Destroy();
  pipeline.Destroy();
  renderPass.Destroy();
  swapChain.Destroy();
  device.Destroy();
  if (surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(backend.GetVkInstance(), surface, nullptr);
    surface = VK_NULL_HANDLE;
  }
  backend.Destroy();
}

}  // namespace Glaceon
