#pragma once
#include "Geometry.h"
#include "Attrib.h"

namespace Graphics {

	Geometry<PositionAttrib, NormalAttrib> CreateQuad();
}

//namespace Graphics {
//
//	struct QuadVertex {
//		float x, y, z;
//		float r, g, b;
//		float nx, ny, nz;
//		float u, v;
//	};
//
//class QuadGeometry : public Geometry<QuadVertex, PositionAttrib, ColorAttrib, NormalAttrib, TexcoordAttrib> {
//public:
//	QuadGeometry();
//	~QuadGeometry();
//
//private:
//	void init();
//};
//
//}

