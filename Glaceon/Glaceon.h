#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include "pch.h"

#include "Application.h"
#include "Base.h"

namespace glaceon {

void GLACEON_API RunGame(Application *app);

// asset pointers
TriangleMesh *triangle_mesh = nullptr;

}// namespace glaceon
#endif// GLACEON_GLACEON_GLACEON_H_
