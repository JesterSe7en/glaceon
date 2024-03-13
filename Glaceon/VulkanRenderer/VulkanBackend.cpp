#include "VulkanBackend.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {

VulkanBackend::VulkanBackend(VulkanContext &context) : context(context) {}

void VulkanBackend::Initialize() {
  VkInstanceCreateInfo instanceCreateInfo = {};
  instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceCreateInfo.pNext = nullptr;
  instanceCreateInfo.flags = 0;

#if _DEBUG
  uint32_t layerCount;
  std::vector<VkLayerProperties> layerProperties;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  layerProperties.resize(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

  const char *validationLayers[] = {"VK_LAYER_KHRONOS_validation"};

  if (IsLayerAvailable(layerProperties, "VK_LAYER_KHRONOS_validation")) {
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = validationLayers;
  } else {
    GERROR("VK_LAYER_KHRONOS_validation layer not available");
  }

  VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
  debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  debugCreateInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
  debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  debugCreateInfo.pfnUserCallback = debugCallback;
  debugCreateInfo.pUserData = nullptr;
  instanceCreateInfo.pNext = &debugCreateInfo;

  context.AddInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#else
  instanceCreateInfo.enabledLayerCount = 0;
  instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

  context.AddInstanceExtension(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
  context.AddInstanceExtension(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

  PopulateInstanceExtensions();

  auto instanceExtensions = context.GetInstanceExtensions();
  instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
  instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();
  VkInstance vkInstance = VK_NULL_HANDLE;
  VkResult res = vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
  if (res != VK_SUCCESS) {
    GERROR("Failed to create Vulkan instance");
    return;
  } else {
    GINFO("Successfully created vulkan instance");
  }

  context.SetVulkanInstance(vkInstance);
}

void VulkanBackend::PopulateInstanceExtensions() {
  uint32_t property_count = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);
  std::vector<VkExtensionProperties> properties;
  properties.resize(property_count);
  vkEnumerateInstanceExtensionProperties(nullptr, &property_count, properties.data());

  std::vector<const char *> instanceExtensions = context.GetInstanceExtensions();

  // check if extensions are available, remove if not
  for (auto &extension : instanceExtensions) {
    if (!IsExtensionAvailable(properties, extension)) {
      GERROR("Extension {} not available", extension);
      context.RemoveInstanceExtension(extension);
    }
  }
}

bool VulkanBackend::IsLayerAvailable(const std::vector<VkLayerProperties> &layers, const char *layer) {
  for (const auto &l : layers) {
    if (strcmp(l.layerName, layer) == 0) {
      return true;
    }
  }
  return false;
}

bool VulkanBackend::IsExtensionAvailable(const std::vector<VkExtensionProperties> &extensions, const char *extension) {
  for (const auto &ext : extensions) {
    if (strcmp(ext.extensionName, extension) == 0) {
      return true;
    }
  }
  return false;
}

VkBool32 VulkanBackend::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                      [[maybe_unused]] void *pUserData) {
  switch (messageSeverity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      GTRACE("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      GINFO("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      GWARN("Validation layer: {}", pCallbackData->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
      GERROR("Validation layer: {}", pCallbackData->pMessage);
      break;
    default:
      GERROR("Validation layer: {}", pCallbackData->pMessage);
      break;
  }

  // per spec, needs to return VK_FALSE
  return VK_FALSE;
}

}  // namespace Glaceon
