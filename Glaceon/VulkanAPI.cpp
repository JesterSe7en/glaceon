#include "VulkanAPI.h"

#include <vulkan/vulkan_core.h>

#include <cassert>

#include "Logger.h"

namespace Glaceon {

VkInstance VulkanAPI::vkInstance = VK_NULL_HANDLE;
VkDevice VulkanAPI::vkDevice = VK_NULL_HANDLE;
VkQueue VulkanAPI::vkQueue = VK_NULL_HANDLE;
VkDescriptorPool VulkanAPI::vkDescriptorPool = VK_NULL_HANDLE;
VkPipelineCache VulkanAPI::vkPipelineCache = VK_NULL_HANDLE;
// std::shared_ptr<VkInstance> VulkanAPI::p_vkInstance = VK_NULL_HANDLE;

// -------- Vulkan API Helper Functions --------
static bool IsExtensionAvailable(const std::vector<VkExtensionProperties> &extensions, const char *extension) {
  for (const auto &ext : extensions) {
    if (strcmp(ext.extensionName, extension) == 0) {
      return true;
    }
  }
  return false;
}

static void debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                          const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, void *pUserData) {
  GLACEON_LOG_ERROR("Validation layer: {}", pCallbackData->pMessage);
}

static VkPhysicalDevice GetPhysicalDevice() {
  if (VulkanAPI::getVulkanInstance() == VK_NULL_HANDLE) {
    GLACEON_LOG_ERROR("Vulkan instance not initialized; cannot get physical device");
    return VK_NULL_HANDLE;
  }

  uint32_t gpu_count = 0;
  std::vector<VkPhysicalDevice> gpus;
  int res = vkEnumeratePhysicalDevices(VulkanAPI::getVulkanInstance(), &gpu_count, nullptr);

  if (res != VK_SUCCESS) {
    GLACEON_LOG_ERROR("Failed to poll number of physical devices");
    assert(gpu_count > 0);
    return VK_NULL_HANDLE;
  }

  gpus.resize(gpu_count);
  res = vkEnumeratePhysicalDevices(VulkanAPI::getVulkanInstance(), &gpu_count, gpus.data());
  if (res != VK_SUCCESS) {
    GLACEON_LOG_ERROR("Failed to enumerate physical devices info");
    return VK_NULL_HANDLE;
  }

  for (VkPhysicalDevice &device : gpus) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);
    if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return device;
  }

  if (gpu_count > 0) return gpus[0];
  return VK_NULL_HANDLE;
}

static void PrintPhysicalDevice(VkPhysicalDevice gpu) {
  if (gpu == VK_NULL_HANDLE) {
    GLACEON_LOG_ERROR("Cannot print physical device info: invalid handle");
    return;
  }

  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(gpu, &properties);

  GLACEON_LOG_INFO("Physical device name: {}", properties.deviceName);
  GLACEON_LOG_INFO("API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion),
                   VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));
  GLACEON_LOG_INFO("Driver version: {}", properties.driverVersion);
  GLACEON_LOG_INFO("Vendor ID: {}", properties.vendorID);
  GLACEON_LOG_INFO("Device ID: {}", properties.deviceID);
}

// -------- Vulkan API Class --------
void VulkanAPI::initVulkan(std::vector<const char *> instance_extensions) {
  {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // FIXME: Add this only for debug builds
    //    const char *validation_layers[] = {"VK_LAYER_KHRONOS_profiles",
    //    "VK_LAYER_KHRONOS_validation"}; createInfo.enabledLayerCount = 2;
    //    createInfo.ppEnabledLayerNames = validation_layers;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
    debugCreateInfo.pUserData = nullptr;
    createInfo.pNext = &debugCreateInfo;

    uint32_t properties_count;
    std::vector<VkExtensionProperties> properties;
    vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
    properties.resize(properties_count);
    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, properties.data());

    if (res != VK_SUCCESS) {
      GLACEON_LOG_ERROR("Failed to enumerate instance extensions");
      return;
    }

    if (!IsExtensionAvailable(properties, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
      GLACEON_LOG_ERROR("Failed to find KHR_get_physical_device_properties_2 extension");
      return;
    } else {
      instance_extensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (IsExtensionAvailable(properties, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
      instance_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    createInfo.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size());
    createInfo.ppEnabledExtensionNames = instance_extensions.data();
    //    res = vkCreateInstance(&createInfo, nullptr, p_vkInstance.get());
    res = vkCreateInstance(&createInfo, nullptr, &vkInstance);
    if (res != VK_SUCCESS) {
      GLACEON_LOG_ERROR("Failed to create Vulkan instance");
      return;
    } else {
      GLACEON_LOG_INFO("Vulkan instance created successfully");
    }
  }

  vkPhysicalDevice = GetPhysicalDevice();
  PrintPhysicalDevice(vkPhysicalDevice);
  vkGraphicsQueueFamilyIndex = (uint32_t)-1;
  {
    uint32_t count;
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &count, nullptr);
    VkQueueFamilyProperties *queues = (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * count);
    vkGetPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, &count, queues);
    for (uint32_t i = 0; i < count; i++)
      if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        vkGraphicsQueueFamilyIndex = i;
        break;
      }
    free(queues);
    assert(vkGraphicsQueueFamilyIndex != (uint32_t)-1);
  }

  {
    std::vector<const char *> device_extensions;
    device_extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    uint32_t properties_count;
    std::vector<VkExtensionProperties> properties;
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, nullptr);
    properties.resize(properties_count);
    vkEnumerateDeviceExtensionProperties(vkPhysicalDevice, nullptr, &properties_count, properties.data());

    const float queue_priority[] = {0.0f};
    VkDeviceQueueCreateInfo queueCreateInfo[1] = {};
    queueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo[0].queueFamilyIndex = vkGraphicsQueueFamilyIndex;
    queueCreateInfo[0].queueCount = 1;
    queueCreateInfo[0].pQueuePriorities = queue_priority;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = sizeof(queueCreateInfo) / sizeof(queueCreateInfo[0]);
    createInfo.pQueueCreateInfos = queueCreateInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
    createInfo.ppEnabledExtensionNames = device_extensions.data();
    int res = vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice);
    if (res != VK_SUCCESS) {
      GLACEON_LOG_ERROR("Failed to create Vulkan device");
      return;
    } else {
      GLACEON_LOG_INFO("Vulkan device created successfully");
    }
    vkGetDeviceQueue(vkDevice, vkGraphicsQueueFamilyIndex, 0, &vkQueue);
  }

  {
    VkDescriptorPoolSize pool_sizes[] = {
        {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = pool_sizes;
    int res = vkCreateDescriptorPool(vkDevice, &pool_info, nullptr, &vkDescriptorPool);
    if (res != VK_SUCCESS) {
      GLACEON_LOG_ERROR("Failed to create descriptor pool");
    } else {
      GLACEON_LOG_INFO("Descriptor pool created successfully");
    }
  }
}

}  // namespace Glaceon
