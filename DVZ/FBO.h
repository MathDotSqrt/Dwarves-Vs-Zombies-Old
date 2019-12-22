#pragma once
#include "common.h"
#include "TEX.h"

namespace Graphics {
	class FBO {
	private:
		uint32 fboID = 0;
		TEX color;
		TEX depth;

	public:
		FBO(const TEX::Builder &builder1);
		FBO(int width, int height);
		FBO(const FBO &) = delete;
		FBO(FBO&& other);
		~FBO();

		FBO& operator=(const FBO &) = delete;
		FBO& operator&=(FBO&&);

		void dispose();

		void bind();
		void unbind();

		TEX& getColorAttachment();
		TEX& getDepthAttachment();

		int getWidth();
		int getHeight();

	};
}


