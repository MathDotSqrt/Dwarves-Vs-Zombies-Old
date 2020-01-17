#pragma once
#include <GL/glew.h>
#include <string>
#include "macrologger.h"
#include "stb_image.h"


namespace Graphics {

class TEX {

public:
	GLenum textureTarget;
	int width;
	int height;

	TEX(TEX&&);
	~TEX();
	TEX(const TEX&) = delete;
	TEX& operator=(const TEX&) = delete;
	TEX& operator=(TEX &&other);

	void bind();
	void bindActiveTexture(int unit);
	void unbind();

	void dispose();

	GLuint getTexID();

	class Builder {
	private:
		friend class TEX;
		std::string filename;
		int width, height;

		GLuint texID;
		GLenum textureTarget;

		GLenum storage;
		GLenum components;
		GLenum dataType;

		GLenum wrapS;
		GLenum wrapT;
		float color[4];

		GLenum filter;
		GLenum mipmap;
		bool useMipMap;

		
	public:
		Builder(int width, int height);
		Builder(std::string filename);
		~Builder();
		Builder& rgb();
		Builder& rgba();
		Builder& depth24();
		Builder& unsignedByteType();
		Builder& floatType();

		Builder& repeat();
		Builder& mirrorRepeat();
		Builder& clampToEdge();
		Builder& clampToBorder();
		Builder& borderColor(float r, float g, float b, float a);
		Builder& nearest();
		Builder& linear();
		Builder& mipmapNearest();
		Builder& mipmapLinear();

		TEX buildTexture();
		TEX buildTextureAtlasArray(int rows, int cols);
	};


private:
	TEX(Builder &builder);
	GLuint texID;
};

}
