#pragma once
#include "Geometry.h"

namespace Graphics {

class QuadGeometry : public Geometry
{
public:
	QuadGeometry();
	~QuadGeometry();

private:
	void init();
};

}

