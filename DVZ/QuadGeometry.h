#pragma once
#include "Geometry.h"
#include "Attrib.h"

namespace Graphics {

	struct QuadVertex {
		float x, y, z;
		float r, g, b;
		float nx, ny, nz;
		float u, v;
	};

class QuadGeometry : public Geometry<
	QuadVertex,
	Attrib<POSITION_ATTRIB_LOCATION, glm::vec3>,
	Attrib<COLOR_ATTRIB_LOCATION, glm::vec3>,
	Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3>,
	Attrib<TEXCOORD_ATTRIB_LOCATION, glm::vec2>
> {
public:
	QuadGeometry();
	~QuadGeometry();

private:
	void init();
};

}

