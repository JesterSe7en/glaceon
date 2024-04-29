#include "VulkanDescriptorPool.h"

#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"

// maybe refactor this as a descritpor manager?
// initalizes if set not avaialble upon request?
// create discrete pools based on usage?
// only ask for set layout when initializing?

namespace glaceon {
VulkanDescriptorPool::VulkanDescriptorPool(VulkanContext &context) : context_(context) {}

VulkanDescriptorPool::~VulkanDescriptorPool() { Destroy(); }

// Initializes/creates the descriptor set layout, descriptor pool, and descriptor sets
void VulkanDescriptorPool::Initialize(const std::vector<DescriptorPoolSetLayoutParams> &params) {
  VK_ASSERT(!context_.GetVulkanSwapChain().GetSwapChainFrames().empty(), "Swap chain not initialized");
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");
  this->descriptor_pool_set_layout_params_ = params;

  // Define first how the shape of descriptor sets will look like
  CreateDescriptorSetLayouts();
  CreateDescriptorPool();
  CreateDescriptorSet();
}

void VulkanDescriptorPool::CreateDescriptorSetLayouts() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  // i.e. it is kind of like an interface.  Only describes how the data should be shaped.

  for (const auto &kParam : descriptor_pool_set_layout_params_) {
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    bindings.reserve(kParam.binding_count);
    for (int i = 0; i < kParam.binding_count; i++) {
      vk::DescriptorSetLayoutBinding binding = {};
      binding.binding = kParam.binding_index[i];
      binding.descriptorType = kParam.descriptor_type[i];
      binding.descriptorCount = kParam.descriptor_type_count[i];
      binding.stageFlags = kParam.stage_to_bind[i];
      bindings.push_back(binding);
    }

    vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
    descriptor_set_layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
    descriptor_set_layout_info.pNext = nullptr;
    descriptor_set_layout_info.flags = vk::DescriptorSetLayoutCreateFlags();
    descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptor_set_layout_info.pBindings = bindings.data();

    vk::DescriptorSetLayout descriptor_set_layout = nullptr;
    VK_CHECK(device.createDescriptorSetLayout(&descriptor_set_layout_info, nullptr, &descriptor_set_layout),
             "Failed to create descriptor set layout");
    vk_descriptor_set_layouts_.insert(std::make_pair(kParam.descriptor_pool_type, descriptor_set_layout));
  }
}

void VulkanDescriptorPool::CreateDescriptorPool() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  for (auto &params : descriptor_pool_set_layout_params_) {

    // Determine the size of the pool
    std::vector<vk::DescriptorPoolSize> pool_sizes;// This stores the type and number of descriptors
    for (int i = 0; i < params.binding_count; i++) {
      vk::DescriptorPoolSize pool_size;
      pool_size.type = params.descriptor_type[i];
      pool_size.descriptorCount = params.set_count;
      pool_sizes.push_back(pool_size);
    }

    // Allocating the right size of descriptor pool
    vk::DescriptorPoolCreateInfo pool_create_info = {};
    pool_create_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
    pool_create_info.pNext = nullptr;
    pool_create_info.flags = vk::DescriptorPoolCreateFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
    pool_create_info.maxSets = params.set_count;
    pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_create_info.pPoolSizes = pool_sizes.data();
    // We only have one pool (aka size of pool_size[])
    // The Image_Sampler pool only allows for one allocation.
    // The VkDescriptorPool itself will only allow for one descriptor set to be allocated aka maxSets

    vk::DescriptorPool vk_descriptor_pool = nullptr;
    VK_CHECK(device.createDescriptorPool(&pool_create_info, nullptr, &vk_descriptor_pool), "Failed to create descriptor pool");
    vk_descriptor_pools_.insert(std::make_pair(params.descriptor_pool_type, vk_descriptor_pool));
  }
}

void VulkanDescriptorPool::CreateDescriptorSet() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  for (auto &params : descriptor_pool_set_layout_params_) {
    if (params.descriptor_pool_type == DescriptorPoolType::IMGUI) {
      // let imgui allocate the set
      continue;
    }
    std::vector<vk::DescriptorSet> all_sets;
    for (int i = 0; i < params.set_count; i++) {
      vk::DescriptorSetAllocateInfo allocate_info = {};
      allocate_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
      allocate_info.pNext = nullptr;
      allocate_info.descriptorPool = vk_descriptor_pools_[params.descriptor_pool_type];
      allocate_info.descriptorSetCount = 1;
      allocate_info.pSetLayouts = &vk_descriptor_set_layouts_[params.descriptor_pool_type];
      vk::DescriptorSet vk_descriptor_set = nullptr;
      VK_CHECK(device.allocateDescriptorSets(&allocate_info, &vk_descriptor_set), "Failed to allocate descriptor set");
      all_sets.push_back(vk_descriptor_set);
    }
    vk_descriptor_sets_.insert(std::make_pair(params.descriptor_pool_type, all_sets));
  }
}

void VulkanDescriptorPool::Destroy() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Vulkan Descriptor Pool not destroyed; cannot find logical device");

  for (auto &kSetLayout : vk_descriptor_set_layouts_) {
    if (kSetLayout.second != VK_NULL_HANDLE) { device.destroyDescriptorSetLayout(kSetLayout.second, nullptr); }
  }

  for (auto &kPool : vk_descriptor_pools_) {
    if (kPool.second != VK_NULL_HANDLE) { device.destroyDescriptorPool(kPool.second, nullptr); }
  }
}
}// namespace glaceon
