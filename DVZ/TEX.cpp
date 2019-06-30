#include "TEX.h"

using namespace Graphics;

TEX::TEX(Builder *builder) : 
	texID(builder->texID),
	textureTarget(builder->textureTarget),
	width(builder->width),
	height(builder->height){

	builder->~Builder();
}


TEX::~TEX()
{
	LOG_INFO("Deleting texture: %d", this->texID);
	glDeleteTextures(1, &this->texID);
}

void TEX::bind() {
	glBindTexture(this->textureTarget, this->texID);
}

void TEX::bindActiveTexture(int unit) {
	if (unit >= 0) {
		glActiveTexture(GL_TEXTURE0 + unit);
		this->bind();
	}
}

void TEX::unbind() {
	glBindTexture(this->textureTarget, 0);
}

TEX::Builder::Builder(std::string filename) {
	this->filename = filename;
	this->repeat()->nearest()->borderColor(0, 0, 0, 1);
}

TEX::Builder::~Builder() {
}

TEX::Builder* TEX::Builder::repeat() {
	this->wrapS = GL_REPEAT;
	this->wrapT = GL_REPEAT;
	return this;
}

TEX::Builder* TEX::Builder::mirrorRepeat() {
	this->wrapS = GL_MIRRORED_REPEAT;
	this->wrapT = GL_MIRRORED_REPEAT;
	return this;
}

TEX::Builder* TEX::Builder::clampToEdge() {
	this->wrapS = GL_CLAMP_TO_EDGE;
	this->wrapT = GL_CLAMP_TO_EDGE;
	return this;
}

TEX::Builder* TEX::Builder::clampToBorder() {
	this->wrapS = GL_CLAMP_TO_BORDER;
	this->wrapT = GL_CLAMP_TO_BORDER;
	return this;
}

TEX::Builder* TEX::Builder::borderColor(float r, float g, float b, float a) {
	this->color[0] = r;
	this->color[1] = g;
	this->color[2] = b;
	this->color[3] = a;
	return this;
}

TEX::Builder* TEX::Builder::nearest() {
	this->filter = GL_NEAREST;
	return this;
}

TEX::Builder* TEX::Builder::linear() {
	this->filter = GL_LINEAR;
	return this;
}

TEX::Builder* TEX::Builder::mipmapNearest() {
	this->mipmap = GL_LINEAR;
	this->useMipMap = true;
	return this;
}

TEX::Builder* TEX::Builder::mipmapLinear() {
	this->mipmap = GL_LINEAR;
	this->useMipMap = true;
	return this;
}

TEX TEX::Builder::buildTexture() {
	int channels;
	unsigned char *image = stbi_load(this->filename.c_str(), &this->width, &this->height, &channels, 3);
	this->textureTarget = GL_TEXTURE_2D;
	glGenTextures(1, &this->texID);
	glBindTexture(this->textureTarget, this->texID);
	glTexImage2D(this->textureTarget, 0, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameteri(this->textureTarget, GL_TEXTURE_WRAP_S, this->wrapS);
	glTexParameteri(this->textureTarget, GL_TEXTURE_WRAP_T, this->wrapT);
	glTexParameterfv(this->textureTarget, GL_TEXTURE_BORDER_COLOR, this->color);

	GLenum f;
	
	if (this->useMipMap) {
		glGenerateMipmap(this->textureTarget);
		unsigned char bit1 = this->mipmap == GL_LINEAR;
		unsigned char bit2 = this->filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
			case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
			case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
			case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
			case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = this->filter;
	}

	glTexParameteri(this->textureTarget, GL_TEXTURE_MIN_FILTER, this->filter);
	glTexParameteri(this->textureTarget, GL_TEXTURE_MAG_FILTER, this->filter);

	stbi_image_free(image);

	glBindTexture(this->textureTarget, 0);

	LOG_INFO("Creating Texture:{\n\tname: %s, texture: %d, width: %d, height: %d\n}", this->filename.c_str(), this->texID, this->width, this->height);
	return TEX(this);
}