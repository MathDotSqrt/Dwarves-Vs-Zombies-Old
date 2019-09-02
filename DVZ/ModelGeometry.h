#pragma once
#include "Geometry.h"
#include "Attrib.h"
#include <string.h>


namespace Graphics {

	struct ModelVertex {
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};

	class ModelGeometry : public Geometry<
		ModelVertex, 
		Attrib<POSITION_ATTRIB_LOCATION, glm::vec3>, 
		Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3>,
		Attrib<TEXCOORD_ATTRIB_LOCATION, glm::vec2>
	> {
	private:

	
	public:
		ModelGeometry(std::string filename);
		~ModelGeometry();
	};
}

