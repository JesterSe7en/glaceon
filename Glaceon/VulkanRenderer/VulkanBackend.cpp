#include "VulkanBackend.h"

#include "../Core/Logger.h"
#include "VulkanContext.h"

namespace glaceon {

VulkanBackend::VulkanBackend(VulkanContext &context) : context_(context) {}

VulkanBackend::~VulkanBackend() { Destroy(); }

void VulkanBackend::Initialize() {
  std::vector<const char *> requested_extensions;

  vk::InstanceCreateInfo instance_create_info;
  instance_create_info.sType = vk::StructureType::eInstanceCreateInfo;
  instance_create_info.pNext = nullptr;
  instance_create_info.flags = vk::InstanceCreateFlags();

#if _DEBUG
  // Enable validation layers
  uint32_t layer_count;
  std::vector<vk::LayerProperties> layer_properties;
  (void) vk::enumerateInstanceLayerProperties(&layer_count, nullptr);
  layer_properties.resize(layer_count);
  (void) vk::enumerateInstanceLayerProperties(&layer_count, layer_properties.data());

  std::vector<const char *> validation_layers = {"VK_LAYER_KHRONOS_validation"};

  // Check if all validation layers are available
  // if not remove it from the vector
  for (const char *layer : validation_layers) {
    if (!IsLayerAvailable(layer_properties, layer)) {
      GERROR("Validation layer {} not available", layer);
      std::erase(validation_layers, layer);
    }
  }

  instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
  instance_create_info.ppEnabledLayerNames = validation_layers.data();

  vk::DebugUtilsMessengerCreateInfoEXT debug_create_info;
  debug_create_info.sType = vk::StructureType::eDebugUtilsMessengerCreateInfoEXT;
  debug_create_info.pNext = nullptr;
  debug_create_info.flags = vk::DebugUtilsMessengerCreateFlagsEXT();
  debug_create_info.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
  debug_create_info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
      | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
  debug_create_info.pfnUserCallback = DebugCallback;
  debug_create_info.pUserData = nullptr;
  instance_create_info.pNext = &debug_create_info;

  requested_extensions.push_back(vk::EXTDebugUtilsExtensionName);

#else
  instance_create_info.enabledLayerCount = 0;
  instance_create_info.ppEnabledLayerNames = nullptr;
#endif

  requested_extensions.push_back(vk::KHRPortabilityEnumerationExtensionName);

  uint32_t property_count = 0;
  (void) vk::enumerateInstanceExtensionProperties(nullptr, &property_count, nullptr);
  std::vector<vk::ExtensionProperties> properties;
  properties.resize(property_count);
  (void) vk::enumerateInstanceExtensionProperties(nullptr, &property_count, properties.data());

  // glfw has some required extensions, this gets set during runGame()
  std::vector<const char *> instance_extensions = context_.GetInstanceExtensions();
  // add requested extensions
  instance_extensions.insert(instance_extensions.end(), requested_extensions.begin(), requested_extensions.end());

  // check if extensions are available, remove if not
  for (auto &extension : instance_extensions) {
    if (!IsExtensionAvailable(properties, extension)) {
      GERROR("Extension {} not available", extension);
      std::erase(instance_extensions, extension);
    }
  }
  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size());
  instance_create_info.ppEnabledExtensionNames = instance_extensions.data();

  if (vk::createInstance(&instance_create_info, nullptr, &instance_) != vk::Result::eSuccess) {
    GERROR("Failed to create Vulkan instance");
    return;
  }
  GINFO("Successfully created Vulkan instance");
}

VkBool32 VulkanBackend::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT message_type,
                                      const VkDebugUtilsMessengerCallbackDataEXT *p_callback_data, [[maybe_unused]] void *p_user_data) {
  switch (message_severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
      GTRACE("Validation layer: {}", p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
      GINFO("Validation layer: {}", p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
      GWARN("Validation layer: {}", p_callback_data->pMessage);
      break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
    default:
      GERROR("Validation layer: {}", p_callback_data->pMessage);
      break;
  }

  // per spec, needs to return VK_FALSE
  return VK_FALSE;
}

void VulkanBackend::Destroy() {
  if (instance_ != VK_NULL_HANDLE) {
    instance_.destroy();
    instance_ = VK_NULL_HANDLE;
  }
}
bool VulkanBackend::IsLayerAvailable(const std::vector<vk::LayerProperties> &layers, const char *layer_to_check) {
  for (const auto &kLayer : layers) {
    if (strcmp(kLayer.layerName, layer_to_check) == 0) { return true; }
  }
  return false;
}
bool VulkanBackend::IsExtensionAvailable(const std::vector<vk::ExtensionProperties> &all_extensions, const char *extension_to_check) {
  for (const auto &kExt : all_extensions) {
    if (strcmp(kExt.extensionName, extension_to_check) == 0) { return true; }
  }
  return false;
}
}// namespace glaceon
