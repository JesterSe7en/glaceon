#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_

namespace glaceon {

class VulkanContext;

struct DescriptorPoolSetLayoutParams {
  int binding_count;                                 // Total number of bindings
  std::vector<int> binding_index;                    // Or indices the binding is describing
  std::vector<vk::DescriptorType> descriptor_type;   // e.g. uniform buffer or storage buffer, etc.
  std::vector<int> descriptor_type_count;            // Number of descriptors of each type
  std::vector<vk::ShaderStageFlagBits> stage_to_bind;// Stage to bind to
};

class VulkanDescriptorPool {
 public:
  explicit VulkanDescriptorPool(VulkanContext &context);
  ~VulkanDescriptorPool();

  void Initialize(const DescriptorPoolSetLayoutParams &params);

  vk::DescriptorSetLayout &GetVkDescriptorSetLayout() { return vk_descriptor_set_layout_; }
  vk::DescriptorPool &GetVkDescriptorPool() { return vk_descriptor_pool_; }

 private:
  VulkanContext &context_;
  DescriptorPoolSetLayoutParams descriptor_pool_set_layout_params_;

  vk::DescriptorSetLayout vk_descriptor_set_layout_;
  // This describes how data in a descriptor set should be laid out
  vk::DescriptorPool vk_descriptor_pool_;

  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void CreateDescriptorSet();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
