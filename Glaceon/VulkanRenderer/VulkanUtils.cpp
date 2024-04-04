#include "VulkanUtils.h"
#include "../Base.h"
#include "../Logger.h"

namespace glaceon {
std::vector<char> VulkanUtils::ReadFile(const std::string &filename) {
  std::vector<char> buffer;
  try {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
      GERROR("Failed to open file: {}", filename);
      return buffer;
    }

    size_t file_size = (size_t) file.tellg();
    buffer.resize(file_size);

    file.seekg(0);
    file.read(buffer.data(), file_size);

    file.close();
  } catch (std::exception &e) {
    GERROR("Exception caught while reading file: {}", e.what());
    buffer.clear();
  }
  return buffer;
}

vk::ShaderModule VulkanUtils::CreateShaderModule(vk::Device const &device, const std::vector<char> &code) {
  vk::ShaderModule shader_module = VK_NULL_HANDLE;
  try {
    if (code.empty()) {
      GERROR("Cannot create shader module: Shader code is empty");
      return VK_NULL_HANDLE;
    }

    if (!device) {
      GERROR("Cannot create shader module: Device is null");
      return VK_NULL_HANDLE;
    }

    vk::ShaderModuleCreateInfo shader_module_create_info = {};
    shader_module_create_info.sType = vk::StructureType::eShaderModuleCreateInfo;
    shader_module_create_info.codeSize = code.size();
    shader_module_create_info.pCode = reinterpret_cast<const uint32_t *>(code.data());

    if (device.createShaderModule(&shader_module_create_info, nullptr, &shader_module) != vk::Result::eSuccess) {
      GERROR("Failed to create shader module");
      return VK_NULL_HANDLE;
    }
  } catch (std::exception &e) {
    GERROR("Exception caught while creating shader module: {}", e.what());
    if (shader_module != VK_NULL_HANDLE) {
      device.destroy(shader_module);
      shader_module = VK_NULL_HANDLE;
    }
  }
  return shader_module;
}

vk::ShaderModule VulkanUtils::CreateShaderModule(vk::Device const &device, const std::string &filename) {
  std::vector<char> code = ReadFile(filename);
  if (code.empty()) { return VK_NULL_HANDLE; }
  return CreateShaderModule(device, code);
}

glaceon::VulkanUtils::Buffer VulkanUtils::CreateBuffer(VulkanUtils::BufferInputParams params) {
  Buffer buffer;
  vk::Device device = params.device;
  // Provided by VK_VERSION_1_0
  //  typedef struct VkBufferCreateInfo {
  //    VkStructureType        sType;
  //    const void*            pNext;
  //    VkBufferCreateFlags    flags;
  //    VkDeviceSize           size;
  //    VkBufferUsageFlags     usage;
  //    VkSharingMode          sharingMode;
  //    uint32_t               queueFamilyIndexCount;
  //    const uint32_t*        pQueueFamilyIndices;
  //  } VkBufferCreateInfo;
  vk::BufferCreateInfo buffer_create_info = {};
  buffer_create_info.sType = vk::StructureType::eBufferCreateInfo;
  buffer_create_info.pNext = nullptr;
  buffer_create_info.flags = vk::BufferCreateFlags();
  buffer_create_info.size = params.size;
  buffer_create_info.usage = params.buffer_usage;
  buffer_create_info.sharingMode = vk::SharingMode::eExclusive;

  buffer.buffer = device.createBuffer(buffer_create_info);

  //check memory requirements of buffer
  int memory_index = FindMemoryIndex(params, buffer.buffer);
  if (memory_index < 0) {
    GERROR("Failed to find memory index");
    return {};
  }

  // Provided by VK_VERSION_1_0
  //  typedef struct VkMemoryAllocateInfo {
  //    VkStructureType    sType;
  //    const void*        pNext;
  //    VkDeviceSize       allocationSize;
  //    uint32_t           memoryTypeIndex;
  //  } VkMemoryAllocateInfo;
  vk::MemoryAllocateInfo memory_allocate_info = {};
  memory_allocate_info.sType = vk::StructureType::eMemoryAllocateInfo;
  memory_allocate_info.pNext = nullptr;
  memory_allocate_info.allocationSize = params.size;
  memory_allocate_info.memoryTypeIndex = memory_index;

  buffer.buffer_memory = device.allocateMemory(memory_allocate_info);
  device.bindBufferMemory(buffer.buffer, buffer.buffer_memory, 0);
  return buffer;
}

int VulkanUtils::FindMemoryIndex(VulkanUtils::BufferInputParams &params, vk::Buffer buffer) {
  vk::Device device = params.device;
  vk::PhysicalDevice physical_device = params.physical_device;

  // requirements from the buffer, these need to be available from the GPU
  vk::MemoryRequirements memory_requirements = device.getBufferMemoryRequirements(buffer);
  // The size member of the VkMemoryRequirements structure returned from a call to vkGetBufferMemoryRequirements with buffer must be less than or equal to the size of memory minus memoryOffset
  if (params.size < memory_requirements.size) { params.size = memory_requirements.size; }

  // these are the types of memory that are available on the GPU, check memory type - stored as a bitfield
  vk::PhysicalDeviceMemoryProperties memory_properties = physical_device.getMemoryProperties();

  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((memory_requirements.memoryTypeBits & (1 << i))
        && (memory_properties.memoryTypes[i].propertyFlags & params.memory_property_flags)) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

void VulkanUtils::CopyBuffer(VulkanUtils::Buffer &src, VulkanUtils::Buffer &dst, vk::DeviceSize size, vk::Queue queue,
                             vk::CommandBuffer command_buffer) {
  // command buffer should be the main one

  command_buffer.reset();

  vk::CommandBufferBeginInfo command_buffer_begin_info = {};
  command_buffer_begin_info.sType = vk::StructureType::eCommandBufferBeginInfo;
  command_buffer_begin_info.flags = vk::CommandBufferUsageFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
  command_buffer_begin_info.pNext = nullptr;
  command_buffer.begin(command_buffer_begin_info);

  // create copyregion with vk::BuFferCopy
  vk::BufferCopy copy_region = {};
  copy_region.srcOffset = 0;
  copy_region.dstOffset = 0;
  copy_region.size = size;
  command_buffer.copyBuffer(src.buffer, dst.buffer, 1, &copy_region);

  command_buffer.end();

  vk::SubmitInfo submit_info = {};
  submit_info.sType = vk::StructureType::eSubmitInfo;
  submit_info.pNext = nullptr;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &command_buffer;

  VK_CHECK(queue.submit(1, &submit_info, nullptr), "Failed to submit command buffer - copy buffer");
  queue.waitIdle();
}

void VulkanUtils::DestroyBuffer(VulkanUtils::BufferInputParams params, VulkanUtils::Buffer &buffer) {
  vk::Device device = params.device;
  VK_ASSERT(device != VK_NULL_HANDLE, "Failed to get Vulkan logical device");

  device.destroy(buffer.buffer, nullptr);
  device.freeMemory(buffer.buffer_memory, nullptr);
  buffer.buffer = VK_NULL_HANDLE;
  buffer.buffer_memory = VK_NULL_HANDLE;
}
uint32_t VulkanUtils::GetMemoryIndex(uint32_t bits, vk::Flags<vk::MemoryPropertyFlagBits> flags) {
  vk::PhysicalDeviceMemoryProperties memory_properties = vk_physical_device_.getMemoryProperties();
  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((bits & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & flags)) {
      return i;
    }
  }
  return -1;
}

}// namespace glaceon
