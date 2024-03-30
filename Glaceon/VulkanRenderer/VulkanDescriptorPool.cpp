
#include "VulkanDescriptorPool.h"
#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"

namespace glaceon {
VulkanDescriptorPool::VulkanDescriptorPool(VulkanContext &context) : context_(context) {}

// Initializes/creates the descriptor set layout, descriptor pool, and descriptor sets
void VulkanDescriptorPool::Initialize(const DescriptorPoolSetLayoutParams &params) {
  VK_ASSERT(!context_.GetVulkanSwapChain().GetSwapChainFrames().empty(), "Swap chain not initialized");
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");
  this->descriptor_pool_set_layout_params_ = params;

  // Define first how the shape of descriptor sets will look like
  CreateDescriptorSetLayout();
  CreateDescriptorPool();
  CreateDescriptorSet();
}

void VulkanDescriptorPool::CreateDescriptorSetLayout() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  // i.e. it is kind of like an interface.  Only describes how the data should be shaped.

  // Provided by VK_VERSION_1_0
  //  typedef struct VkDescriptorSetLayoutCreateInfo {
  //    VkStructureType                        sType;
  //    const void*                            pNext;
  //    VkDescriptorSetLayoutCreateFlags       flags;
  //    uint32_t                               bindingCount;
  //    const VkDescriptorSetLayoutBinding*    pBindings;
  //  } VkDescriptorSetLayoutCreateInfo;

  std::vector<vk::DescriptorSetLayoutBinding> bindings;
  bindings.reserve(descriptor_pool_set_layout_params_.binding_count);
  for (int i = 0; i < descriptor_pool_set_layout_params_.binding_count; i++) {
    // Provided by VK_VERSION_1_0
    //    typedef struct VkDescriptorSetLayoutBinding {
    //      uint32_t              binding;
    //      VkDescriptorType      descriptorType;
    //      uint32_t              descriptorCount;
    //      VkShaderStageFlags    stageFlags;
    //      const VkSampler*      pImmutableSamplers;
    //    } VkDescriptorSetLayoutBinding;
    vk::DescriptorSetLayoutBinding binding = {};
    binding.binding = descriptor_pool_set_layout_params_.binding_index[i];
    binding.descriptorType = descriptor_pool_set_layout_params_.descriptor_type[i];
    binding.descriptorCount = descriptor_pool_set_layout_params_.descriptor_type_count[i];
    binding.stageFlags = descriptor_pool_set_layout_params_.stage_to_bind[i];
    bindings.push_back(binding);
  }

  vk::DescriptorSetLayoutCreateInfo descriptor_set_layout_info = {};
  descriptor_set_layout_info.sType = vk::StructureType::eDescriptorSetLayoutCreateInfo;
  descriptor_set_layout_info.pNext = nullptr;
  descriptor_set_layout_info.flags = vk::DescriptorSetLayoutCreateFlags();
  descriptor_set_layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
  descriptor_set_layout_info.pBindings = bindings.data();

  VK_CHECK(device.createDescriptorSetLayout(&descriptor_set_layout_info, nullptr, &vk_descriptor_set_layout_),
           "Failed to create descriptor set layout");
  GINFO("Successfully created descriptor set layout");
}

void VulkanDescriptorPool::CreateDescriptorPool() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");
  // Determine the size of the pool
  uint8_t num_descriptor_sets = 0;
  std::vector<vk::DescriptorPoolSize> pool_sizes;// This stores the type and number of descriptors
  for (int i = 0; i < descriptor_pool_set_layout_params_.binding_count; i++) {
    vk::DescriptorPoolSize pool_size;
    pool_size.type = descriptor_pool_set_layout_params_.descriptor_type[i];
    pool_size.descriptorCount = descriptor_pool_set_layout_params_.descriptor_type_count[i];
    num_descriptor_sets++;
    pool_sizes.push_back(pool_size);
  }

#if _DEBUG
  // Add image sampler to the pool for ImGui
  vk::DescriptorPoolSize image_sampler;
  image_sampler.type = vk::DescriptorType::eCombinedImageSampler;
  image_sampler.descriptorCount = 1;
  num_descriptor_sets++;
  pool_sizes.push_back(image_sampler);
#endif

  // Allocating the right size of descriptor pool
  vk::DescriptorPoolCreateInfo pool_create_info = {};
  pool_create_info.sType = vk::StructureType::eDescriptorPoolCreateInfo;
  pool_create_info.pNext = nullptr;
  pool_create_info.flags = vk::DescriptorPoolCreateFlags(vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet);
  pool_create_info.maxSets = static_cast<uint32_t>(num_descriptor_sets);
  pool_create_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
  pool_create_info.pPoolSizes = pool_sizes.data();
  // We only have one pool (aka size of pool_size[])
  // The Image_Sampler pool only allows for one allocation.
  // The VkDescriptorPool itself will only allow for one descriptor set to be allocated aka maxSets
  VK_CHECK(device.createDescriptorPool(&pool_create_info, nullptr, &vk_descriptor_pool_),
           "Failed to create descriptor pool");
  GINFO("Successfully created descriptor pool");
}

void VulkanDescriptorPool::CreateDescriptorSet() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  vk::DescriptorSetAllocateInfo allocate_info = {};
  allocate_info.sType = vk::StructureType::eDescriptorSetAllocateInfo;
  allocate_info.pNext = nullptr;
  allocate_info.descriptorPool = vk_descriptor_pool_;
  allocate_info.descriptorSetCount = 1;
  allocate_info.pSetLayouts = &vk_descriptor_set_layout_;
  VK_CHECK(device.allocateDescriptorSets(&allocate_info, &vk_descriptor_set_), "Failed to allocate descriptor set");
  GINFO("Successfully allocated descriptor set");
//
//  auto &swap_chain_frames = context_.GetVulkanSwapChain().GetSwapChainFrames();
//  std::vector<vk::DescriptorSetLayout> layouts(swap_chain_frames.size(), vk_descriptor_set_layout_);
//  vk::DescriptorSetAllocateInfo alloc_info(vk_descriptor_pool_, static_cast<uint32_t>(swap_chain_frames.size()),
//                                           layouts.data());
//
//  std::vector<vk::DescriptorSet> descriptor_sets(swap_chain_frames.size());
//  VK_CHECK(device.allocateDescriptorSets(&alloc_info, descriptor_sets.data()), "Failed to allocate descriptor sets");
//
//  for (size_t i = 0; i < swap_chain_frames.size(); i++) {
//    swap_chain_frames[i].descriptor_set = descriptor_sets[i];
//    GINFO("Successfully allocated descriptor set for frame {}", i);
//  }
}
void VulkanDescriptorPool::Destroy() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE,
            "Descriptor pool not destroyed; cannot find logical device");
  if (vk_descriptor_pool_ != VK_NULL_HANDLE) {
    device.destroy(vk_descriptor_pool_);
    vk_descriptor_pool_ = VK_NULL_HANDLE;
  }

  if (vk_descriptor_set_layout_ != VK_NULL_HANDLE) {
    device.destroy(vk_descriptor_set_layout_);
    vk_descriptor_set_layout_ = VK_NULL_HANDLE;
  }
}
}// namespace glaceon