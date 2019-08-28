#pragma once
#include "Geometry.h"

namespace Graphics {

	struct QuadVertex {
		float x, y, z;
		float r, g, b;
		float nx, ny, nz;
		float u, v;
	};

class QuadGeometry : public Geometry<QuadVertex> {
public:
	QuadGeometry();
	~QuadGeometry();

private:
	void init();
};

}

