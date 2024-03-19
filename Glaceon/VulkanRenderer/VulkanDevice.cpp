#include "VulkanDevice.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace Glaceon {

VulkanDevice::VulkanDevice(VulkanContext &context) : context_(context) {
  queue_indexes_.graphics_family = std::nullopt;
  queue_indexes_.present_family = std::nullopt;
}

void VulkanDevice::Initialize() {
  GINFO("Initializing Vulkan device...");
  if (context_.GetVulkanInstance() == VK_NULL_HANDLE) {
    GERROR("Vulkan instance not initialized; cannot initialize device")
    return;
  }

  vk::Instance instance = context_.GetVulkanInstance();

  uint32_t gpu_count = 0;
  std::vector<vk::PhysicalDevice> gpus;
  if (instance.enumeratePhysicalDevices(&gpu_count, nullptr) != VK_SUCCESS) {
    GERROR("Failed to poll number of physical devices")
    assert(gpu_count > 0);
    return;
  }
  gpus.resize(gpu_count);
  if (instance.enumeratePhysicalDevices(&gpu_count, gpus.data()) != VK_SUCCESS) {
    GERROR("Failed to enumerate physical devices info")
    return;
  }

  for (vk::PhysicalDevice &gpu : gpus) {
    if (CheckDeviceRequirements(gpu)) {
      vk_physical_device_ = gpu;
      break;
    }
  }

  if (vk_physical_device_ == VK_NULL_HANDLE) {
    GERROR("Failed to find a suitable GPU");
    return;
  } else {
    GINFO("Successfully found discrete GPU");
    PrintPhysicalDevice(vk_physical_device_);
  }

  const float kQueuePriority[] = {1.0f};

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
  create_info.queueCreateInfoCount = queue_create_info.size();
  create_info.pQueueCreateInfos = queue_create_info.data();
  create_info.enabledExtensionCount = context_.GetDeviceExtensions().size();
  create_info.ppEnabledExtensionNames = context_.GetDeviceExtensions().data();

  if (vk_physical_device_.createDevice(&create_info, nullptr, &vk_device_) != vk::Result::eSuccess) {
    GERROR("Failed to create Vulkan device");
    return;
  } else {
    GINFO("Successfully created Vulkan device");
  }

  vk_device_.getQueue(queue_indexes_.graphics_family.value(), 0, &vk_graphics_queue_);
  vk_device_.getQueue(queue_indexes_.present_family.value(), 0, &vk_present_queue_);

  // create command pool for graphics queue - command pool only needs VkDevice to create it
  vk::CommandPoolCreateInfo pool_info = {};
  pool_info.sType = vk::StructureType::eCommandPoolCreateInfo;
  pool_info.queueFamilyIndex = queue_indexes_.graphics_family.value();
  pool_info.flags = vk::CommandPoolCreateFlags(vk::CommandPoolCreateFlagBits::eResetCommandBuffer);

  if (vk_device_.createCommandPool(&pool_info, nullptr, &vk_command_pool_) != vk::Result::eSuccess) {
    GERROR("Failed to create graphics command pool");
    return;
  } else {
    GINFO("Successfully created graphics command pool");
  }

  vk::DescriptorPoolSize pool_size[] = {
      {vk::DescriptorType::eCombinedImageSampler, 1},
  };

  vk::DescriptorPoolCreateInfo pool_create_info = {};
  pool_create_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  pool_create_info.poolSizeCount = 1;
  pool_create_info.pPoolSizes = pool_size;
  pool_create_info.maxSets = 1;
  pool_create_info.flags = vk::DescriptorPoolCreateFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
  // We only have one pool (aka size of pool_size[])
  // The Image_Sampler pool only allows for one allocation.
  // The VkDescriptorPool itself will only allow for one descriptor set to be allocated aka maxSets
  if (vk_device_.createDescriptorPool(&pool_create_info, nullptr, &vk_descriptor_pool_) != vk::Result::eSuccess) {
    GERROR("Failed to create descriptor pool");
    return;
  } else {
    GINFO("Successfully created descriptor pool");
  }
}

bool VulkanDevice::CheckDeviceRequirements(vk::PhysicalDevice &vk_physical_device) {
  vk::Instance instance = context_.GetVulkanInstance();
  vk::PhysicalDeviceProperties properties = vk_physical_device.getProperties();

  if (properties.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
    GTRACE("Device is not a discrete GPU, skipping...");
    return false;
  }

  uint32_t queue_family_count;
  queue_family_ = vk_physical_device.getQueueFamilyProperties();

#if _DEBUG
  for (uint32_t i = 0; i < queue_family_count; i++) {
    GTRACE("Queue family {} properties:", i);
    GTRACE("  Queue count: {}", queue_family_[i].queueCount);
    GTRACE("  Supports graphics: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eGraphics ? "true" : "false");
    GTRACE("  Supports compute: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eCompute ? "true" : "false");
    GTRACE("  Supports transfer: {}", queue_family_[i].queueFlags & vk::QueueFlagBits::eTransfer ? "true" : "false");
    GTRACE("  Supports sparse binding: {}",
           queue_family_[i].queueFlags & vk::QueueFlagBits::eSparseBinding ? "true" : "false");
  }
#endif

  vk::SurfaceKHR surface = context_.GetSurface();
  assert(surface != VK_NULL_HANDLE);

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
  } else {
    GTRACE("Device supports graphics and presentation queue families");
  }

  uint32_t properties_count;
  (void) vk_physical_device.enumerateDeviceExtensionProperties(nullptr, &properties_count, nullptr);
  device_extensions_.resize(properties_count);
  (void) vk_physical_device.enumerateDeviceExtensionProperties(nullptr, &properties_count, device_extensions_.data());

  // check device extensions required by the engine (in our case it is VK_KHR_SWAPCHAIN)
  std::vector<const char *> extensions = context_.GetDeviceExtensions();

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
    if (strcmp(kExtension.extensionName, ext) == 0) {
      return true;
    }
  }
  return false;
}

void VulkanDevice::PrintPhysicalDevice(vk::PhysicalDevice gpu) {
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
  if (vk_descriptor_pool_ != VK_NULL_HANDLE) {
    vk_device_.destroy(vk_descriptor_pool_, nullptr);
    vk_descriptor_pool_ = VK_NULL_HANDLE;
  }

  if (vk_command_pool_ != VK_NULL_HANDLE) {
    vk_device_.destroy(vk_command_pool_, nullptr);
    vk_command_pool_ = VK_NULL_HANDLE;
  }

  if (vk_device_ != VK_NULL_HANDLE) {
    vk_device_.destroy();
    vk_device_ = VK_NULL_HANDLE;
  }
}

}  // namespace Glaceon
