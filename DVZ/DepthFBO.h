#pragma once
#include "FBO.h"
#include "TEX.h"

namespace Graphics {
	class DepthFBO : public FBO {
	private:
		TEX depth;

	public:
		DepthFBO(int width, int height);
		DepthFBO(const DepthFBO &) = delete;
		DepthFBO(DepthFBO &&);
		~DepthFBO();

		DepthFBO& operator&=(DepthFBO &&other);

		TEX& getDepthAttachment();
	};
}

