#include "VulkanDevice.h"

#include "../Logger.h"
#include "VulkanContext.h"

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
    if (CheckDeviceRequirements(device)) {
      physicalDevice = device;
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
  uint32_t index = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);

  VkDeviceQueueCreateInfo queueCreateInfo = {};
  //  typedef struct VkDeviceQueueCreateInfo {
  //    VkStructureType             sType;
  //    const void*                 pNext;
  //    VkDeviceQueueCreateFlags    flags;
  //    uint32_t                    queueFamilyIndex;
  //    uint32_t                    queueCount;
  //    const float*                pQueuePriorities;
  //  } VkDeviceQueueCreateInfo;
  queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueCreateInfo.pNext = nullptr;
  queueCreateInfo.flags = 0;
  queueCreateInfo.queueFamilyIndex = index;
  queueCreateInfo.queueCount = 1;
  float queuePriority = 1.0f;
  queueCreateInfo.pQueuePriorities = &queuePriority;

  VkDeviceCreateInfo deviceCreateInfo = {};
  //  typedef struct VkDeviceCreateInfo {
  //    VkStructureType                    sType;
  //    const void*                        pNext;
  //    VkDeviceCreateFlags                flags;
  //    uint32_t                           queueCreateInfoCount;
  //    const VkDeviceQueueCreateInfo*     pQueueCreateInfos;
  //    uint32_t                           enabledLayerCount;
  //    const char* const*                 ppEnabledLayerNames;
  //    uint32_t                           enabledExtensionCount;
  //    const char* const*                 ppEnabledExtensionNames;
  //    const VkPhysicalDeviceFeatures*    pEnabledFeatures;
  //  } VkDeviceCreateInfo;
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.pNext = nullptr;
  deviceCreateInfo.flags = 0;
  deviceCreateInfo.queueCreateInfoCount = 1;
  deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
  deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
  deviceCreateInfo.ppEnabledExtensionNames = context.GetDeviceExtensions().data();

  //  res = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
  //  if (res != VK_SUCCESS) {
  //    GERROR("Failed to create Vulkan device");
  //    return;
  //  } else {
  //    GINFO("Vulkan device created successfully");
  //  }

  //  vkGetDeviceQueue(device, index, 0, &queue);

  // #if _DEBUG
  //   for (uint32_t i = 0; i < queueFamilyCount; i++) {
  //     GTRACE("Queue family {} properties:", i);
  //     GTRACE("  Queue count: {}", queueFamily[i].queueCount);
  //     GTRACE("  Supports graphics: {}", queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? "true" : "false");
  //     GTRACE("  Supports compute: {}", queueFamily[i].queueFlags & VK_QUEUE_COMPUTE_BIT ? "true" : "false");
  //     GTRACE("  Supports transfer: {}", queueFamily[i].queueFlags & VK_QUEUE_TRANSFER_BIT ? "true" : "false");
  //     GTRACE("  Supports sparse binding: {}", queueFamily[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? "true" :
  //     "false"); GTRACE("  Supports protected: {}", queueFamily[i].queueFlags & VK_QUEUE_PROTECTED_BIT ? "true" :
  //     "false"); GTRACE("  Supports presentation: {}", queueFamily[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? "true" :
  //     "false");
  //   }
  // #endif
}

bool VulkanDevice::CheckDeviceRequirements(VkPhysicalDevice &vkPhysicalDevice) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(vkPhysicalDevice, &properties);

  if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
    GTRACE("Device is not a discrete GPU, skipping...");
    return false;
  }

  uint32_t count;
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &count, nullptr);
  this->queueFamily.resize(count);
  vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &count, queueFamily.data());

  // check if device extensions are available
  uint32_t properties_count;
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, nullptr);
  deviceExtensions.resize(properties_count);
  vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, deviceExtensions.data());

  std::vector<const char *> extensions = context.GetDeviceExtensions();

  for (const char *ext : extensions) {
    if (!IsExtensionAvailable(properties, ext)) {
      GTRACE("Device extension {} not available, skipping...", ext);
      context.RemoveInstanceExtension(ext);
    }
  }
  if (extensions.empty() == 0) {
    GTRACE("No device extensions available, skipping...");
    return false;
  }

  GTRACE("Vulkan device requirements met");
  return true;
}

VulkanDevice::VulkanDevice(VulkanContext &context) : context(context) {}
bool VulkanDevice::IsExtensionAvailable(VkPhysicalDeviceProperties properties, const char *ext) {
  for (const auto &extension : deviceExtensions) {
    if (strcmp(extension.extensionName, ext) == 0) {
      return true;
    }
  }
  return false;
}

uint32_t VulkanDevice::GetQueueFamilyIndex(VkQueueFlagBits bits) {
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