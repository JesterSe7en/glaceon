#include "VulkanTexture.h"

#include "../Logger.h"
#include "VulkanContext.h"

namespace glaceon {

VulkanTexture::VulkanTexture(VulkanContext &context, const char *filename) : context_(context), filename_(filename) {
  pixels_ = stbi_load(filename, &width_, &height_, &channels_, 0);
}

void VulkanTexture::Use() {}

VulkanTexture::~VulkanTexture() { stbi_image_free(pixels_); }
}// namespace glaceon