#include "VulkanTexture.h"
#include <cstring>
#define STB_IMAGE_IMPLEMENTATION
#include "../Base.h"
#include "../Logger.h"
#include "VulkanContext.h"
#include <stb_image.h>

namespace glaceon {

VulkanTexture::VulkanTexture(VulkanContext &context, vk::DescriptorSet target_descriptor_set, const char *filename)
    : width_(0), height_(0), channels_(0), pixels_(nullptr), context_(context), filename_(filename), vk_descriptor_set_(target_descriptor_set) {
  LoadImageFromFile();
  CreateVkImage();
  Populate();
  CreateVkImageView();
  CreateSampler();
  UpdateDescriptorSet();
}

void VulkanTexture::LoadImageFromFile() {
  VK_ASSERT(filename_ != nullptr, "Texture filename is null");
  pixels_ = stbi_load(filename_, &width_, &height_, &channels_, STBI_rgb_alpha);
  if (pixels_ == nullptr) { GWARN("Failed to load texture: {}", filename_); }
}

// Creates the Vulkan image and allocates memory for it
void VulkanTexture::CreateVkImage() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  vk::ImageCreateInfo image_info = {};
  image_info.sType = vk::StructureType::eImageCreateInfo;
  image_info.pNext = nullptr;
  image_info.flags = vk::ImageCreateFlags();
  image_info.imageType = vk::ImageType::e2D;
  image_info.format = vk::Format::eR8G8B8A8Unorm;
  image_info.extent.width = width_;
  image_info.extent.height = height_;
  image_info.extent.depth = 1;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.samples = vk::SampleCountFlagBits::e1;
  image_info.tiling = vk::ImageTiling::eOptimal;
  // need both usage flags as we are transferring from cpu to gpu, and will be sampled from in shader code
  image_info.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
  image_info.sharingMode = vk::SharingMode::eExclusive;
  image_info.initialLayout = vk::ImageLayout::eUndefined;

  VK_CHECK(device.createImage(&image_info, nullptr, &vk_image_), "Failed to create image");

  // Back the VkImage with memory
  vk::MemoryRequirements memory_requirements = {};
  device.getImageMemoryRequirements(vk_image_, &memory_requirements);

  vk::MemoryAllocateInfo memory_allocate_info = {};
  memory_allocate_info.sType = vk::StructureType::eMemoryAllocateInfo;
  memory_allocate_info.pNext = nullptr;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = VulkanUtils::FindMemoryTypeIndex(context_.GetVulkanPhysicalDevice(), memory_requirements.memoryTypeBits,
                                                                          vk::MemoryPropertyFlagBits::eDeviceLocal);
  VK_CHECK(device.allocateMemory(&memory_allocate_info, nullptr, &vk_image_memory_), "Failed to allocate image memory");

  device.bindImageMemory(vk_image_, vk_image_memory_, 0);
}

void VulkanTexture::CreateVkImageView() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  vk::ImageViewCreateInfo image_view_info = {};
  image_view_info.sType = vk::StructureType::eImageViewCreateInfo;
  image_view_info.pNext = nullptr;
  image_view_info.flags = vk::ImageViewCreateFlags();
  image_view_info.image = vk_image_;
  image_view_info.viewType = vk::ImageViewType::e2D;
  image_view_info.format = vk::Format::eR8G8B8A8Unorm;
  image_view_info.components.r = vk::ComponentSwizzle::eIdentity;
  image_view_info.components.g = vk::ComponentSwizzle::eIdentity;
  image_view_info.components.b = vk::ComponentSwizzle::eIdentity;
  image_view_info.components.a = vk::ComponentSwizzle::eIdentity;
  image_view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  image_view_info.subresourceRange.baseMipLevel = 0;
  image_view_info.subresourceRange.levelCount = 1;
  image_view_info.subresourceRange.baseArrayLayer = 0;
  image_view_info.subresourceRange.layerCount = 1;

  VK_CHECK(device.createImageView(&image_view_info, nullptr, &vk_image_view_), "Failed to create image view");
}

void VulkanTexture::Populate() {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  VulkanUtils::BufferInputParams params = {};
  params.device = context_.GetVulkanLogicalDevice();
  params.physical_device = context_.GetVulkanPhysicalDevice();
  params.memory_property_flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
  params.buffer_usage = vk::BufferUsageFlags(vk::BufferUsageFlagBits::eTransferSrc);
  params.size = width_ * height_ * 4;

  VulkanUtils::Buffer staging_buffer = VulkanUtils::CreateBuffer(params);
  void *staging_buffer_mapped = device.mapMemory(staging_buffer.buffer_memory, 0, params.size);

#ifdef __linux__
  memcpy(staging_buffer_mapped, pixels_, params.size);
#else
  memcpy_s(staging_buffer_mapped, params.size, pixels_, params.size);
#endif

  TransitionImageLayout(vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
  CopyBufferToImage(staging_buffer.buffer, vk_image_);
  TransitionImageLayout(vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);

  VulkanUtils::DestroyBuffer(params, staging_buffer);
}

void VulkanTexture::CopyBufferToImage(vk::Buffer &src_buffer, vk::Image &dst_image) {
  vk::Device device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");
  vk::CommandBuffer command_buffer = context_.GetVulkanCommandPool().GetVkMainCommandBuffer();
  VK_ASSERT(command_buffer != VK_NULL_HANDLE, "Main command buffer not initialized");

  VulkanUtils::BeginSingleTimeCommands(command_buffer);

  vk::BufferImageCopy buffer_image_copy = {};
  buffer_image_copy.bufferOffset = 0;
  buffer_image_copy.bufferRowLength = 0;
  buffer_image_copy.bufferImageHeight = 0;

  vk::ImageSubresourceLayers image_subresource_layers = {};
  image_subresource_layers.aspectMask = vk::ImageAspectFlagBits::eColor;
  image_subresource_layers.mipLevel = 0;
  image_subresource_layers.baseArrayLayer = 0;
  image_subresource_layers.layerCount = 1;
  buffer_image_copy.imageSubresource = image_subresource_layers;

  buffer_image_copy.imageOffset = vk::Offset3D(0, 0, 0);
  buffer_image_copy.imageExtent = vk::Extent3D(width_, height_, 1);

  command_buffer.copyBufferToImage(src_buffer, dst_image, vk::ImageLayout::eTransferDstOptimal, 1, &buffer_image_copy);
  VulkanUtils::EndSingleTimeCommands(command_buffer, context_.GetVulkanDevice().GetVkGraphicsQueue());
}

void VulkanTexture::TransitionImageLayout(vk::ImageLayout old_layout, vk::ImageLayout new_layout) {
  vk::Device device = context_.GetVulkanLogicalDevice();
  vk::CommandBuffer command_buffer = context_.GetVulkanCommandPool().GetVkMainCommandBuffer();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");
  VK_ASSERT(command_buffer != VK_NULL_HANDLE, "Main command buffer not initialized");

  VulkanUtils::BeginSingleTimeCommands(command_buffer);

  vk::ImageMemoryBarrier image_memory_barrier = {};
  image_memory_barrier.sType = vk::StructureType::eImageMemoryBarrier;
  image_memory_barrier.oldLayout = old_layout;
  image_memory_barrier.newLayout = new_layout;
  image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  image_memory_barrier.image = vk_image_;

  // which bit of the image we are looking at?
  image_memory_barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
  image_memory_barrier.subresourceRange.baseMipLevel = 0;
  image_memory_barrier.subresourceRange.levelCount = 1;
  image_memory_barrier.subresourceRange.baseArrayLayer = 0;
  image_memory_barrier.subresourceRange.layerCount = 1;

  vk::PipelineStageFlags src_stage_mask, dst_stage_mask;
  if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal) {
    image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eNoneKHR;
    image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

    src_stage_mask = vk::PipelineStageFlagBits::eTopOfPipe;
    dst_stage_mask = vk::PipelineStageFlagBits::eTransfer;
  } else {
    image_memory_barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
    image_memory_barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

    src_stage_mask = vk::PipelineStageFlagBits::eTransfer;
    dst_stage_mask = vk::PipelineStageFlagBits::eFragmentShader;
  }
  command_buffer.pipelineBarrier(src_stage_mask, dst_stage_mask, vk::DependencyFlags(), 0, nullptr, 0, nullptr, 1, &image_memory_barrier);
  VulkanUtils::EndSingleTimeCommands(command_buffer, context_.GetVulkanDevice().GetVkGraphicsQueue());
}

void VulkanTexture::CreateSampler() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  vk::SamplerCreateInfo sampler_info = {};
  sampler_info.sType = vk::StructureType::eSamplerCreateInfo;
  sampler_info.pNext = nullptr;
  sampler_info.flags = vk::SamplerCreateFlags();
  sampler_info.magFilter = vk::Filter::eLinear;
  sampler_info.minFilter = vk::Filter::eNearest;
  sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
  sampler_info.addressModeU = vk::SamplerAddressMode::eRepeat;// these tell the sampler how to sample when out of bounds in the u, v, w axes
  sampler_info.addressModeV = vk::SamplerAddressMode::eRepeat;
  sampler_info.addressModeW = vk::SamplerAddressMode::eRepeat;
  sampler_info.mipLodBias = 0.0f;
  sampler_info.anisotropyEnable = VK_FALSE;
  sampler_info.maxAnisotropy = 1.0f;
  sampler_info.compareEnable = VK_FALSE;
  sampler_info.compareOp = vk::CompareOp::eAlways;
  sampler_info.minLod = 0.0f;
  sampler_info.maxLod = 0.0f;
  sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
  sampler_info.unnormalizedCoordinates = VK_FALSE;
  VK_CHECK(device.createSampler(&sampler_info, nullptr, &vk_sampler_), "Failed to create sampler");
}

void VulkanTexture::UpdateDescriptorSet() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  // combined image sampler
  vk::DescriptorImageInfo descriptor_image_info = {};
  descriptor_image_info.sampler = vk_sampler_;
  descriptor_image_info.imageView = vk_image_view_;
  descriptor_image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;

  vk::WriteDescriptorSet write_descriptor_set = {};
  write_descriptor_set.sType = vk::StructureType::eWriteDescriptorSet;
  write_descriptor_set.dstSet = vk_descriptor_set_;
  write_descriptor_set.dstBinding = 0;
  write_descriptor_set.dstArrayElement = 0;
  write_descriptor_set.descriptorType = vk::DescriptorType::eCombinedImageSampler;
  write_descriptor_set.descriptorCount = 1;
  write_descriptor_set.pImageInfo = &descriptor_image_info;

  device.updateDescriptorSets(1, &write_descriptor_set, 0, nullptr);
}

void VulkanTexture::Use() {
  vk::CommandBuffer command_buffer = context_.GetVulkanCommandPool().GetVkMainCommandBuffer();
  vk::PipelineLayout pipeline_layout = context_.GetVulkanPipeline().GetVkPipelineLayout();
  VK_ASSERT(command_buffer != VK_NULL_HANDLE, "Command buffer not initialized");
  VK_ASSERT(pipeline_layout != VK_NULL_HANDLE, "Pipeline layout not initialized");

  command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipeline_layout, 1, vk_descriptor_set_, nullptr);
}

VulkanTexture::~VulkanTexture() {
  auto device = context_.GetVulkanLogicalDevice();
  VK_ASSERT(device != VK_NULL_HANDLE, "Logical device not initialized");

  if (vk_image_view_ != VK_NULL_HANDLE) {
    device.destroyImageView(vk_image_view_);
    vk_image_view_ = VK_NULL_HANDLE;
  }

  if (vk_sampler_ != VK_NULL_HANDLE) {
    device.destroySampler(vk_sampler_);
    vk_sampler_ = VK_NULL_HANDLE;
  }

  if (vk_image_ != VK_NULL_HANDLE) {
    device.destroyImage(vk_image_);
    vk_image_ = VK_NULL_HANDLE;
  }

  if (vk_image_memory_ != VK_NULL_HANDLE) {
    device.freeMemory(vk_image_memory_);
    vk_image_memory_ = VK_NULL_HANDLE;
  }

  if (pixels_ != nullptr) {
    stbi_image_free(pixels_);
    pixels_ = nullptr;
  }
}
}// namespace glaceon
