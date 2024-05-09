#include "VulkanDevice.h"

#include "../Core/Base.h"
#include "../Core/Logger.h"
#include "VulkanContext.h"

namespace glaceon {

VulkanDevice::VulkanDevice(VulkanContext &context) : context_(context) {
  queue_indexes_.graphics_family = std::nullopt;
  queue_indexes_.present_family = std::nullopt;
}

VulkanDevice::~VulkanDevice() { Destroy(); }

void VulkanDevice::Initialize() {
  GINFO("Initializing Vulkan device...");
  if (context_.GetVulkanInstance() == VK_NULL_HANDLE) {
    GERROR("Vulkan instance not initialized; cannot initialize device");
    return;
  }

  const vk::Instance instance = context_.GetVulkanInstance();

  uint32_t gpu_count = 0;
  std::vector<vk::PhysicalDevice> gpus;
  VK_CHECK(instance.enumeratePhysicalDevices(&gpu_count, nullptr), "Failed to poll number of physical devices");
  VK_ASSERT(gpu_count != 0, "Failed to poll number of physical devices");
  gpus.resize(gpu_count);
  VK_CHECK(instance.enumeratePhysicalDevices(&gpu_count, gpus.data()), "Failed to enumerate physical devices info");

  for (vk::PhysicalDevice &gpu : gpus) {
    if (CheckDeviceRequirements(gpu)) {
      vk_physical_device_ = gpu;
      break;
    }
  }

  if (vk_physical_device_ == VK_NULL_HANDLE) {
    GERROR("Failed to find a suitable GPU");
    return;
  }
  GINFO("Successfully found discrete GPU");
  PrintPhysicalDevice(vk_physical_device_);

  constexpr float kQueuePriority[] = {1.0f};

  std::set<uint32_t> set;
  set.insert(queue_indexes_.graphics_family.value());
  set.insert(queue_indexes_.present_family.value());

  std::vector<vk::DeviceQueueCreateInfo> queue_create_info;
  for (size_t i = 0; i < set.size(); i++) {
    vk::DeviceQueueCreateInfo queue_info = {};
    queue_info.sType = vk::StructureType::eDeviceQueueCreateInfo;
    queue_info.queueFamilyIndex = static_cast<uint32_t>(i);
    queue_info.queueCount = 1;
    queue_info.pQueuePriorities = kQueuePriority;
    queue_create_info.emplace_back(queue_info);
  }

  vk::DeviceCreateInfo create_info = {};
  create_info.sType = vk::StructureType::eDeviceCreateInfo;
  create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_info.size());
  create_info.pQueueCreateInfos = queue_create_info.data();
  create_info.enabledExtensionCount = static_cast<uint32_t>(context_.GetDeviceExtensions().size());
  create_info.ppEnabledExtensionNames = context_.GetDeviceExtensions().data();

  VK_CHECK(vk_physical_device_.createDevice(&create_info, nullptr, &vk_device_), "Failed to create Vulkan device");
  GINFO("Successfully created Vulkan device");

  vk_device_.getQueue(queue_indexes_.graphics_family.value(), 0, &vk_graphics_queue_);
  vk_device_.getQueue(queue_indexes_.present_family.value(), 0, &vk_present_queue_);
}

bool VulkanDevice::CheckDeviceRequirements(const vk::PhysicalDevice &vk_physical_device) {
  if (vk_physical_device.getProperties().deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
    GTRACE("Device is not a discrete GPU, skipping...");
    return false;
  }

  queue_family_ = vk_physical_device.getQueueFamilyProperties();
  const auto queue_family_count = static_cast<uint32_t>(queue_family_.size());

#if _DEBUG
  for (uint32_t i = 0; i < queue_family_count; i++) {
    GTRACE("Queue family {} properties:", i);
    GTRACE("  Queue count: {}", queue_family_[i].queueCount);
    GTRACE("  Supports graphics: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eGraphics ? "true" : "false");
    GTRACE("  Supports compute: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eCompute ? "true" : "false");
    GTRACE("  Supports transfer: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eTransfer ? "true" : "false");
    GTRACE("  Supports sparse binding: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eSparseBinding ? "true" : "false");
  }
#endif

  const vk::SurfaceKHR surface = context_.GetSurface();
  VK_ASSERT(surface != VK_NULL_HANDLE, "Failed to get Vulkan surface");

  // first queue family that supports presentation
  for (uint32_t i = 0; i < queue_family_count; i++) {
    vk::Bool32 present_support = false;
    if (vk_physical_device.getSurfaceSupportKHR(i, surface, &present_support) != vk::Result::eSuccess) {
      GERROR("Failed to get surface support");
      return false;
    }
    if (present_support) {
      queue_indexes_.present_family = i;
      break;
    }
  }
  // first queue family that supports graphics
  for (uint32_t i = 0; i < queue_family_count; i++) {
    if (queue_family_[i].queueFlags & vk::QueueFlagBits::eGraphics) {
      queue_indexes_.graphics_family = i;
      break;
    }
  }

  if (!queue_indexes_.IsComplete()) {
    GTRACE("Device does not support graphics queue family, skipping...");
    return false;
  }
  GTRACE("Device supports graphics and presentation queue families");

  uint32_t properties_count;
  (void) vk_physical_device.enumerateDeviceExtensionProperties(nullptr, &properties_count, nullptr);
  device_extensions_.resize(properties_count);
  (void) vk_physical_device.enumerateDeviceExtensionProperties(nullptr, &properties_count, device_extensions_.data());

  // check device extensions required by the engine (in our case it is VK_KHR_SWAPCHAIN)
  const std::vector<const char *> extensions = context_.GetDeviceExtensions();

  for (const char *ext : extensions) {
    if (!IsExtensionAvailable(ext)) {
      GTRACE("Device extension {} not available, skipping...", ext);
      context_.RemoveInstanceExtension(ext);
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
  for (const auto &kExtension : device_extensions_) {
    if (strcmp(kExtension.extensionName, ext) == 0) { return true; }
  }
  return false;
}

void VulkanDevice::PrintPhysicalDevice(const vk::PhysicalDevice gpu) {
  if (gpu == VK_NULL_HANDLE) {
    GERROR("Cannot print physical device info: invalid handle");
    return;
  }

  const auto &kProperties = gpu.getProperties();
  const auto kMajor = vk::apiVersionMajor(kProperties.apiVersion);
  const auto kMinor = vk::apiVersionMinor(kProperties.apiVersion);
  const auto kPatch = vk::apiVersionPatch(kProperties.apiVersion);

  GINFO("Physical device name: {}", kProperties.deviceName);
  GINFO("API version: {}.{}.{}", kMajor, kMinor, kPatch);
  GINFO("Driver version: {}", kProperties.driverVersion);
  GINFO("Vendor ID: {}", kProperties.vendorID);
  GINFO("Device ID: {}", kProperties.deviceID);
}

void VulkanDevice::Destroy() {
  if (vk_device_ != VK_NULL_HANDLE) {
    vk_device_.destroy();
    vk_device_ = VK_NULL_HANDLE;
  }
}
}// namespace glaceon
