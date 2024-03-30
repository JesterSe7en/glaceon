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
  void Initialize(const DescriptorPoolSetLayoutParams &params);
  void Destroy();

  [[nodiscard]] const vk::DescriptorSetLayout &GetVkDescriptorSetLayout() const { return vk_descriptor_set_layout_; }
  [[nodiscard]] const vk::DescriptorPool &GetVkDescriptorPool() const { return vk_descriptor_pool_; }
  [[nodiscard]] const vk::DescriptorSet &GetVkDescriptorSet() const { return vk_descriptor_set_; }


 private:
  VulkanContext &context_;
  DescriptorPoolSetLayoutParams descriptor_pool_set_layout_params_;

  vk::DescriptorSetLayout vk_descriptor_set_layout_;
  // This describes how data in a descriptor set should be laid out
  vk::DescriptorPool vk_descriptor_pool_;

  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  // i.e. it is kind of like an interface.  Only describes how the data should be shaped.
  vk::DescriptorSet vk_descriptor_set_;

 private:
  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  void CreateDescriptorSetLayout();
  void CreateDescriptorPool();
  void CreateDescriptorSet();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
