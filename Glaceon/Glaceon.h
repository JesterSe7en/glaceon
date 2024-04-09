#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include "pch.h"

#include "Application.h"
#include "Base.h"
#include "VertexBufferCollection.h"
#include "VulkanRenderer/VulkanTexture.h"

namespace glaceon {

void GLACEON_API RunGame(Application *app);

VertexBufferCollection *vertex_buffer_collection = nullptr;
std::unordered_map<MeshType, VulkanTexture *> materials_;

}// namespace glaceon
#endif// GLACEON_GLACEON_GLACEON_H_
