#ifndef GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_
#define GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_

namespace glaceon {

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

class VulkanContext;

class VulkanTexture {
 public:
  explicit VulkanTexture(VulkanContext &context, const char *filename);
  void Use();
  ~VulkanTexture();

 private:
  int width_;
  int height_;
  int channels_;
  const char *filename_;
  stbi_uc *pixels_;
  vk::Device vk_device_;
  vk::PhysicalDevice vk_physical_device_;

  vk::Image vk_image_;
  vk::DeviceMemory vk_image_memory_;
  vk::ImageView vk_image_view_;
  vk::Sampler vk_sampler_;

  vk::DescriptorSetLayout vk_descriptor_set_layout_;
  vk::DescriptorSet vk_descriptor_set_;
  vk::DescriptorPool vk_descriptor_pool_;

  vk::CommandBuffer vk_command_buffer_;
  vk::Queue vk_queue_;

  VulkanContext &context_;
};

}// namespace glaceon

#endif//GLACEON_GLACEON_VULKANRENDERER_VULKANTEXTURE_H_
