#include "VulkanContext.h"

namespace glaceon {

VulkanContext::VulkanContext()
    : backend_(*this),
      device_(*this),
      swap_chain_(*this),
      render_pass_(*this),
      pipeline_(*this),
      command_pool_(*this),
      sync_(*this),
      device_extensions_(),
      instance_extensions_() {}
void VulkanContext::AddDeviceExtension(const char *ext) { device_extensions_.push_back(ext); }
void VulkanContext::AddInstanceExtension(const char *ext) { instance_extensions_.push_back(ext); }
void VulkanContext::RemoveDeviceExtension(const char *ext) {
  auto it = std::find(device_extensions_.begin(), device_extensions_.end(), ext);
  if (it != device_extensions_.end()) {
    device_extensions_.erase(it);
  }
}
void VulkanContext::RemoveInstanceExtension(const char *ext) {
  auto it = std::find(instance_extensions_.begin(), instance_extensions_.end(), ext);
  if (it != instance_extensions_.end()) {
    instance_extensions_.erase(it);
  }
}

void VulkanContext::Destroy() {
  sync_.Destroy();
  command_pool_.Destroy();
  pipeline_.Destroy();
  render_pass_.Destroy();
  swap_chain_.Destroy();
  device_.Destroy();
  if (surface_ != VK_NULL_HANDLE) {
    backend_.GetVkInstance().destroy(surface_, nullptr);
    surface_ = VK_NULL_HANDLE;
  }
  backend_.Destroy();
}

}  // namespace Glaceon
