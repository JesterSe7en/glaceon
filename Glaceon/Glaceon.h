#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include "pch.h"

#include "Application.h"
#include "Base.h"

namespace Glaceon {

void GLACEON_API runGame(Application* app);

void GLACEON_API recordDrawCommands(VkCommandBuffer commandBuffer, uint32_t imageIndex);



}  // namespace Glaceon
#endif  // GLACEON_GLACEON_GLACEON_H_
