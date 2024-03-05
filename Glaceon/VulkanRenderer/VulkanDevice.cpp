#include "VulkanDevice.h"
#include "VulkanContext.h"
#include "../Logger.h"

namespace Glaceon {

void VulkanDevice::Initialize() {
  GINFO("Initializing Vulkan device...");

  if (context.GetVulkanInstance() == VK_NULL_HANDLE) {
    GERROR("Vulkan instance not initialized; cannot initialize device");
    return;
  }

  VkInstance instance = context.GetVulkanInstance();

  uint32_t gpu_count = 0;
  std::vector<VkPhysicalDevice> gpus;
  int res = vkEnumeratePhysicalDevices(instance, &gpu_count, nullptr);

  if (res != VK_SUCCESS) {
    GERROR("Failed to poll number of physical devices");
    assert(gpu_count > 0);
    return;
  }

  gpus.resize(gpu_count);
  res = vkEnumeratePhysicalDevices(instance, &gpu_count, gpus.data());
  if (res != VK_SUCCESS) {
    GERROR("Failed to enumerate physical devices info");
    return;
  }

  for (VkPhysicalDevice &device : gpus) {
    CheckDeviceRequirements(device);
  }
}

bool VulkanDevice::CheckDeviceRequirements(VkPhysicalDevice &physicalDevice) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    GINFO("Device is not a discrete GPU, skipping...");
    return false;
  }

  uint32_t queue_family_count;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, nullptr);
  queue_family.resize(queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queue_family_count, queue_family.data());
}

VulkanDevice::VulkanDevice(VulkanContext &context) : context(context) {}

}  // namespace Glaceon