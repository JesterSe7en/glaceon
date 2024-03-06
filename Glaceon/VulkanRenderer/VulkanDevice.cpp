#include "VulkanDevice.h"

#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {

VulkanDevice::VulkanDevice(VulkanContext &context)
    : physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE), context(context) {}

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

  for (VkPhysicalDevice &vkPhysicalDevice : gpus) {
    if (CheckDeviceRequirements(vkPhysicalDevice)) {
      physicalDevice = vkPhysicalDevice;
      break;
    };
  }

  if (physicalDevice == VK_NULL_HANDLE) {
    GERROR("Failed to find a suitable GPU");
    return;
  } else {
    GINFO("Found discrete GPU");
    PrintPhysicalDevice(physicalDevice);
  }

  // create device with graphic queue
  // find queue family with graphics support
  int index = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
  if (index == -1) {
    GERROR("Failed to find a suitable queue family for {}", string_VkQueueFlagBits(VK_QUEUE_GRAPHICS_BIT));
    return;
  }

  const float queue_priority[] = {0.0f};
  VkDeviceQueueCreateInfo queueCreateInfo[1] = {};
  queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo[0].queueFamilyIndex = index;
  queueCreateInfo[0].queueCount = 1;
  queueCreateInfo[0].pQueuePriorities = queue_priority;

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = sizeof(queueCreateInfo) / sizeof(queueCreateInfo[0]);
  createInfo.pQueueCreateInfos = queueCreateInfo;
  createInfo.enabledExtensionCount = static_cast<uint32_t>(context.GetDeviceExtensions().size());
  createInfo.ppEnabledExtensionNames = context.GetDeviceExtensions().data();

  VkResult success = vkCreateDevice(this->physicalDevice, &createInfo, nullptr, &device);
  if (success != VK_SUCCESS) {
    GERROR("Failed to create Vulkan device");
    return;
  } else {
    GINFO("Vulkan device created successfully");
  }

  vkGetDeviceQueue(device, index, 0, &queue);

  VkDescriptorPoolSize pool_sizes[] = {
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
  };
  VkDescriptorPoolCreateInfo pool_info = {};
  pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
  pool_info.maxSets = 1;
  // We only have one pool (aka size of pool_size[])
  // The Image_Sampler pool only allows for one allocation.
  // The VkDescriptorPool itself will only allow for one descriptor set to be allocated aka maxSets
  pool_info.poolSizeCount = 1;
  pool_info.pPoolSizes = pool_sizes;
  res = vkCreateDescriptorPool(device, &pool_info, nullptr, &descriptorPool);
  if (res != VK_SUCCESS) {
    GERROR("Failed to create descriptor pool");
  } else {
    GINFO("Descriptor pool created successfully");
  }

  context.SetDescriptorPool(descriptorPool);
}

bool VulkanDevice::CheckDeviceRequirements(VkPhysicalDevice &vkPhysicalDevice) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

  if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    GTRACE("Device is not a discrete GPU, skipping...");
    return false;
  }

  uint32_t queueFamilyCount;
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, nullptr);
  this->queueFamily.resize(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &queueFamilyCount, queueFamily.data());

#if _DEBUG
  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    GTRACE("Queue family {} properties:", i);
    GTRACE("  Queue count: {}", queueFamily[i].queueCount);
    GTRACE("  Supports graphics: {}", queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? "true" : "false");
    GTRACE("  Supports compute: {}", queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT ? "true" : "false");
    GTRACE("  Supports transfer: {}", queueFamily[i].queueFlags & VK_QUEUE_TRANSFER_BIT ? "true" : "false");
    GTRACE("  Supports sparse binding: {}", queueFamily[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "true" : "false");
  }
#endif

  for (uint32_t i = 0; i < queueFamilyCount; i++) {
    // Check if the queue family supports graphics
    if (queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      // assuming all graphics queue families support presentation
      graphic_queue_indexes_.graphicsFamily = i;
      graphic_queue_indexes_.presentFamily = i;
      GTRACE("Graphics queue family index: {} supports graphics and present", i);
    }
  }

  if (!graphic_queue_indexes_.isComplete()) {
    GTRACE("Device does not support graphics queue family, skipping...");
    return false;
  } else {
    GTRACE("Device supports graphics queue family");
  }

  uint32_t properties_count;
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, nullptr);
  deviceExtensions.resize(properties_count);
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, deviceExtensions.data());

  std::vector<const char *> extensions = context.GetDeviceExtensions();

  for (const char *ext : extensions) {
    if (!IsExtensionAvailable(ext)) {
      GTRACE("Device extension {} not available, skipping...", ext);
      context.RemoveInstanceExtension(ext);
    }
  }
  if (extensions.empty()) {
    GTRACE("No device extensions available, skipping...");
    return false;
  }

  GTRACE("Vulkan device requirements met");
  return true;
}

bool VulkanDevice::IsExtensionAvailable(const char *ext) {
  for (const auto &extension : deviceExtensions) {
    if (strcmp(extension.extensionName, ext) == 0) {
      return true;
    }
  }
  return false;
}

int VulkanDevice::GetQueueFamilyIndex(VkQueueFlagBits bits) {
  for (uint32_t i = 0; i < queueFamily.size(); i++) {
    if (queueFamily[i].queueFlags & bits) {
      return i;
    }
  }
  return -1;
}

void VulkanDevice::PrintPhysicalDevice(VkPhysicalDevice gpu) {
  if (gpu == VK_NULL_HANDLE) {
    GERROR("Cannot print physical device info: invalid handle");
    return;
  }

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(gpu, &properties);

  GINFO("Physical device name: {}", properties.deviceName);
  GINFO("API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion));
  GINFO("Driver version: {}", properties.driverVersion);
  GINFO("Vendor ID: {}", properties.vendorID);
  GINFO("Device ID: {}", properties.deviceID);
}

}  // namespace Glaceon
