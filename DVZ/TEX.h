#pragma once
#include <GL/glew.h>
#include <string>
#include "macrologger.h"
#include <stb_image.h>

namespace Graphics {

class TEX {

public:
	~TEX();

	const GLuint texID;
	const GLenum textureTarget;
	const int width;
	const int height;

	void bind();
	void bindActiveTexture(int unit);
	void unbind();

	class Builder {
	private:
		friend class TEX;
		std::string filename;
		int width, height;

		GLuint texID;
		GLenum textureTarget;

		GLenum wrapS;
		GLenum wrapT;
		float color[4];

		GLenum filter;
		GLenum mipmap;
		bool useMipMap;

		
	public:
		Builder(std::string filename);
		~Builder();
		Builder* repeat();
		Builder* mirrorRepeat();
		Builder* clampToEdge();
		Builder* clampToBorder();
		Builder* borderColor(float r, float g, float b, float a);
		Builder* nearest();
		Builder* linear();
		Builder* mipmapNearest();
		Builder* mipmapLinear();

		TEX buildTexture();
	};

private:
	TEX(Builder &builder);
};

}
