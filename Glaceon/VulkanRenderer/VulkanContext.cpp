#include "VulkanContext.h"

#include <algorithm>

namespace Glaceon {

VulkanContext::VulkanContext() : backend(*this), device(*this) {}

void VulkanContext::AddInstanceExtension(const char *extension) { this->instanceExtensions.push_back(extension); }

void VulkanContext::RemoveInstanceExtension(const char *extension) {
  this->instanceExtensions.erase(
      std::remove(this->instanceExtensions.begin(), this->instanceExtensions.end(), extension),
      this->instanceExtensions.end());
}

}  // namespace Glaceon