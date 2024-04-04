#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_

namespace glaceon {

class VulkanContext;

enum DescriptorPoolType { FRAME, MESH };

struct DescriptorPoolSetLayoutParams {
  DescriptorPoolType descriptor_pool_type;           // Type of descriptor pool
  int binding_count;                                 // Total number of bindings
  std::vector<int> binding_index;                    // Or indices the binding is describing
  std::vector<vk::DescriptorType> descriptor_type;   // e.g. uniform buffer or storage buffer, etc.
  std::vector<int> descriptor_type_count;            // Number of descriptors of each type
  std::vector<vk::ShaderStageFlagBits> stage_to_bind;// Stage to bind to
};

class VulkanDescriptorPool {
 public:
  explicit VulkanDescriptorPool(VulkanContext &context);
  void Initialize(const std::vector<DescriptorPoolSetLayoutParams> &params);
  void Destroy();

  vk::DescriptorSetLayout &GetDescriptorSetLayout(DescriptorPoolType type) { return vk_descriptor_set_layouts_.at(type); }
  vk::DescriptorPool &GetDescriptorPool(DescriptorPoolType type) { return vk_descriptor_pools_.at(type); }
  vk::DescriptorSet &GetDescriptorSet(DescriptorPoolType type) { return vk_descriptor_sets_.at(type); }

  std::unordered_map<DescriptorPoolType, vk::DescriptorSetLayout> &GetDescriptorSetLayouts() { return vk_descriptor_set_layouts_; }
  std::unordered_map<DescriptorPoolType, vk::DescriptorPool> &GetDescriptorPools() { return vk_descriptor_pools_; }
  std::unordered_map<DescriptorPoolType, vk::DescriptorSet> &GetDescriptorSets() { return vk_descriptor_sets_; }

 private:
  VulkanContext &context_;
  std::vector<DescriptorPoolSetLayoutParams> descriptor_pool_set_layout_params_;

  std::unordered_map<DescriptorPoolType, vk::DescriptorSetLayout> vk_descriptor_set_layouts_;
  std::unordered_map<DescriptorPoolType, vk::DescriptorPool> vk_descriptor_pools_;
  std::unordered_map<DescriptorPoolType, vk::DescriptorSet> vk_descriptor_sets_;

 private:
  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  void CreateDescriptorSetLayouts();
  void CreateDescriptorPool();
  void CreateDescriptorSet();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
