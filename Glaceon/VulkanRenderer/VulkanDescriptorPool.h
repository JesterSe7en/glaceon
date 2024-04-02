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

  [[nodiscard]] const vk::DescriptorSetLayout &GetFrameVkDescriptorSetLayout() const {
    return vk_frame_descriptor_set_layout_;
  }
  [[nodiscard]] const vk::DescriptorPool &GetFrameVkDescriptorPool() const { return vk_frame_descriptor_pool_; }
  [[nodiscard]] const vk::DescriptorSetLayout &GetMeshVkDescriptorSetLayout() const {
    return vk_mesh_descriptor_set_layout_;
  }
  [[nodiscard]] const vk::DescriptorPool &GetMeshVkDescriptorPool() const { return vk_mesh_descriptor_pool_; }

  [[nodiscard]] const vk::DescriptorSet &GetVkDescriptorSet() const { return vk_descriptor_set_; }

 private:
  VulkanContext &context_;
  std::vector<DescriptorPoolSetLayoutParams> descriptor_pool_set_layout_params_;

  vk::DescriptorSetLayout vk_frame_descriptor_set_layout_;
  vk::DescriptorPool vk_frame_descriptor_pool_;

  vk::DescriptorSetLayout vk_mesh_descriptor_set_layout_;
  vk::DescriptorPool vk_mesh_descriptor_pool_;

  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  // i.e. it is kind of like an interface.  Only describes how the data should be shaped.
  vk::DescriptorSet vk_descriptor_set_;

 private:
  // Descriptor Set layout just describes how data in a descriptor set should be laid out
  void CreateDescriptorSetLayouts();
  void CreateDescriptorPool();
  void CreateDescriptorSet();
};

}// namespace glaceon

#endif// GLACEON_GLACEON_VULKANRENDERER_VULKANDESCRIPTORPOOL_H_
