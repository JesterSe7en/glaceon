#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_

namespace glaceon {

class VulkanContext;

struct VulkanTextureInput {
  vk::Format format;
};

class VulkanTexture {
 public:
  VulkanTexture(VulkanContext &context, vk::DescriptorSet target_descriptor_set, const char *filename, const VulkanTextureInput &input);
  ~VulkanTexture();

  void Use(vk::CommandBuffer &command_buffer);

 private:
  int width_;
  int height_;
  int channels_;
  const char *filename_;
  VulkanTextureInput input_;
  unsigned char *pixels_;

  vk::Image vk_image_;
  vk::DeviceMemory vk_image_memory_;
  vk::ImageView vk_image_view_;
  vk::Sampler vk_sampler_;

  vk::DescriptorSet vk_descriptor_set_;

  VulkanContext &context_;

  void LoadImageFromFile();
  void CreateVkImage();
  void CreateVkImageView();
  void TransitionImageLayout(vk::ImageLayout old_layout, vk::ImageLayout new_layout);
  void CreateSampler();
  void UpdateDescriptorSet();
  void Populate();
  void CopyBufferToImage(vk::Buffer &src_buffer, vk::Image &dst_image);
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_
