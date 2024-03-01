//
// Created by alyxc on 3/1/2024.
//

#include "VulkanAPI.h"
#include "Logger.h"
#include <vulkan/vulkan_core.h>

namespace Glaceon {

VkInstance VulkanAPI::vkInstance = VK_NULL_HANDLE;
// std::shared_ptr<VkInstance> VulkanAPI::p_vkInstance = VK_NULL_HANDLE;

static bool
IsExtensionAvailable(const std::vector<VkExtensionProperties> &extensions,
                     const char *extension) {
  for (const auto &ext : extensions) {
    if (strcmp(ext.extensionName, extension) == 0) {
      return true;
    }
  }
  return false;
}

static void
debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
              VkDebugUtilsMessageTypeFlagsEXT messageType,
              const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
              void *pUserData) {
  GLACEON_LOG_ERROR("Validation layer: {}", pCallbackData->pMessage);
}

void VulkanAPI::initVulkan(std::vector<const char *> instance_extensions) {
  {
    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    // FIXME: Add this only for debug builds
    //    const char *validation_layers[] = {"VK_LAYER_KHRONOS_profiles",
    //    "VK_LAYER_KHRONOS_validation"}; createInfo.enabledLayerCount = 2;
    //    createInfo.ppEnabledLayerNames = validation_layers;

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    debugCreateInfo.sType =
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCreateInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debugCreateInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugCreateInfo.pfnUserCallback =
        reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(debugCallback);
    debugCreateInfo.pUserData = nullptr;
    createInfo.pNext = &debugCreateInfo;

    uint32_t properties_count;
    std::vector<VkExtensionProperties> properties;
    vkEnumerateInstanceExtensionProperties(nullptr, &properties_count, nullptr);
    properties.resize(properties_count);
    VkResult res = vkEnumerateInstanceExtensionProperties(
        nullptr, &properties_count, properties.data());

    if (res != VK_SUCCESS) {
      GLACEON_LOG_ERROR("Failed to enumerate instance extensions");
      return;
    }

    if (!IsExtensionAvailable(
            properties,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME)) {
      GLACEON_LOG_ERROR(
          "Failed to find KHR_get_physical_device_properties_2 extension");
      return;
    } else {
      instance_extensions.push_back(
          VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (IsExtensionAvailable(properties,
                             VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) {
      instance_extensions.push_back(
          VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
      createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    }

    createInfo.enabledExtensionCount =
        static_cast<uint32_t>(instance_extensions.size());
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
}

} // namespace Glaceon
