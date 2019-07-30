#pragma once
#include "Geometry.h"

#include <string.h>


namespace Graphics {
	class ModelGeometry : public Geometry {
	private:

	
	public:
		ModelGeometry(std::string filename);
		~ModelGeometry();
	};
}

