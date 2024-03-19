#include "VulkanBackend.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {

VulkanBackend::VulkanBackend(VulkanContext &context) : context(context) {}

void VulkanBackend::Initialize() {
  std::vector<const char *> requestedExtensions;

  vk::InstanceCreateInfo instanceCreateInfo;
  instanceCreateInfo.sType = vk::StructureType::eInstanceCreateInfo;
  instanceCreateInfo.pNext = nullptr;
  instanceCreateInfo.flags = vk::InstanceCreateFlags();

#if _DEBUG
  // Enable validation layers
  uint32_t layerCount;
  std::vector<vk::LayerProperties> layerProperties;
  (void)vk::enumerateInstanceLayerProperties(&layerCount, nullptr);
  layerProperties.resize(layerCount);
  (void)vk::enumerateInstanceLayerProperties(&layerCount, layerProperties.data());

  std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

  // Check if all validation layers are available
  // if not remove it from the vector
  for (const char *layer : validationLayers) {
    if (!IsLayerAvailable(layerProperties, layer)) {
      GERROR("Validation layer {} not available", layer);
      validationLayers.erase(std::remove(validationLayers.begin(), validationLayers.end(), layer),
                             validationLayers.end());
    }
  }

  instanceCreateInfo.enabledLayerCount = validationLayers.size();
  instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

  vk::DebugUtilsMessengerCreateInfoEXT debugCreateInfo;
  debugCreateInfo.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
  debugCreateInfo.pNext = nullptr;
  debugCreateInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
  debugCreateInfo.messageSeverity =
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
  debugCreateInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
  debugCreateInfo.pfnUserCallback = debugCallback;
  debugCreateInfo.pUserData = nullptr;
  instanceCreateInfo.pNext = &debugCreateInfo;

  requestedExtensions.push_back(vk::EXTDebugUtilsExtensionName);

#else
  instanceCreateInfo.enabledLayerCount = 0;
  instanceCreateInfo.ppEnabledLayerNames = nullptr;
#endif

  requestedExtensions.push_back(vk::KHRPortabilityEnumerationExtensionName);

  uint32_t property_count = 0;
  (void)vk::enumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);
  std::vector<vk::ExtensionProperties> properties;
  properties.resize(property_count);
  (void)vk::enumerateInstanceExtensionProperties(nullptr, &property_count, properties.data());

  // glfw has some required extensions, this gets set during runGame()
  std::vector<const char *> instanceExtensions = context.GetInstanceExtensions();

  // check if extensions are available, remove if not
  for (auto &extension : instanceExtensions) {
    if (!IsExtensionAvailable(properties, extension)) {
      GERROR("Extension {} not available", extension);
      instanceExtensions.erase(std::remove(instanceExtensions.begin(), instanceExtensions.end(), extension),
                               instanceExtensions.end());
    }
  }
  instanceCreateInfo.enabledExtensionCount = instanceExtensions.size();
  instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.data();

  if (vk::createInstance(&instanceCreateInfo, nullptr, &instance_) != vk::Result::eSuccess) {
    GERROR("Failed to create Vulkan instance");
    return;
  }
  GINFO("Successfully created Vulkan instance");
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
    default:
      GERROR("Validation layer: {}", pCallbackData->pMessage);
      break;
  }

  // per spec, needs to return VK_FALSE
  return VK_FALSE;
}

void VulkanBackend::Destroy() {
  if (instance_ != VK_NULL_HANDLE) {
    vkDestroyInstance(instance_, nullptr);
    instance_ = VK_NULL_HANDLE;
  }
}
bool VulkanBackend::IsLayerAvailable(const std::vector<vk::LayerProperties> &layers, const char *layerToCheck) {
  for (const auto &l : layers) {
    if (strcmp(l.layerName, layerToCheck) == 0) {
      return true;
    }
  }
  return false;
}
bool VulkanBackend::IsExtensionAvailable(const std::vector<vk::ExtensionProperties> &allExtensions,
                                         const char *extensionToCheck) {
  for (const auto &ext : allExtensions) {
    if (strcmp(ext.extensionName, extensionToCheck) == 0) {
      return true;
    }
  }
  return false;
}
}  // namespace Glaceon
