#ifndef GLACEON_GLACEON_GLACEON_H_
#define GLACEON_GLACEON_GLACEON_H_

#include "pch.h"

#include "Application.h"
#include "Base.h"
#include "VertexBufferCollection.h"

namespace glaceon {

void GLACEON_API RunGame(Application *app);

// asset pointers
TriangleMesh *triangle_mesh = nullptr;
SquareMesh *square_mesh = nullptr;
StarMesh *star_mesh = nullptr;

VertexBufferCollection * vertex_buffer_collection = nullptr;

}// namespace glaceon
#endif// GLACEON_GLACEON_GLACEON_H_
