#include "TEX.h"

using namespace Graphics;

TEX::TEX(TEX &&other) : textureTarget(other.textureTarget), width(other.width), height(other.height){
	texID = other.texID;
}

TEX::TEX(Builder &builder) : 
	texID(builder.texID),
	textureTarget(builder.textureTarget),
	width(builder.width),
	height(builder.height){
}

TEX::~TEX() {
	dispose();
}

TEX& TEX::operator=(TEX &&other) {
	if (this != &other) {
		dispose();

		std::swap(texID, other.texID);
	}

	return *this;
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

void TEX::dispose() {
	if (texID) {
		LOG_INFO("Deleting texture: %d", this->texID);
		glDeleteTextures(1, &this->texID);
		texID = 0;
	}
}

TEX::Builder::Builder(std::string filename) {
	this->filename = filename;
	this->mipmap = false;
	this->useMipMap = false;
	this->rgb().repeat().nearest().borderColor(0, 0, 0, 1);
}

TEX::Builder::~Builder() {

}

TEX::Builder& TEX::Builder::rgb() {
	this->components = GL_RGB;
	this->storage = GL_RGB8;
	return *this;
}

TEX::Builder& TEX::Builder::rgba() {
	this->components = GL_RGBA;
	this->storage = GL_RGBA8;
	return *this;
}

TEX::Builder& TEX::Builder::repeat() {
	this->wrapS = GL_REPEAT;
	this->wrapT = GL_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::mirrorRepeat() {
	this->wrapS = GL_MIRRORED_REPEAT;
	this->wrapT = GL_MIRRORED_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::clampToEdge() {
	this->wrapS = GL_CLAMP_TO_EDGE;
	this->wrapT = GL_CLAMP_TO_EDGE;
	return *this;
}

TEX::Builder& TEX::Builder::clampToBorder() {
	this->wrapS = GL_CLAMP_TO_BORDER;
	this->wrapT = GL_CLAMP_TO_BORDER;
	return *this;
}

TEX::Builder& TEX::Builder::borderColor(float r, float g, float b, float a) {
	this->color[0] = r;
	this->color[1] = g;
	this->color[2] = b;
	this->color[3] = a;
	return *this;
}

TEX::Builder& TEX::Builder::nearest() {
	this->filter = GL_NEAREST;
	return *this;
}

TEX::Builder& TEX::Builder::linear() {
	this->filter = GL_LINEAR;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapNearest() {
	this->mipmap = GL_LINEAR;
	this->useMipMap = true;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapLinear() {
	this->mipmap = GL_LINEAR;
	this->useMipMap = true;
	return *this;
}

TEX TEX::Builder::buildTexture() {
	int channels;
	unsigned char *image = stbi_load(this->filename.c_str(), &this->width, &this->height, &channels, 3);
	this->textureTarget = GL_TEXTURE_2D;
	
	int mipmapLevelCount = 0;


	glGenTextures(1, &this->texID);
	glBindTexture(this->textureTarget, this->texID);
	glTexImage2D(this->textureTarget, mipmapLevelCount, GL_RGB, this->width, this->height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

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

	glTexParameteri(this->textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(this->textureTarget, GL_TEXTURE_MAG_FILTER, this->filter);

	stbi_image_free(image);

	glBindTexture(this->textureTarget, 0);

	LOG_INFO("Creating Texture:{\n\tname: %s, texture: %d, width: %d, height: %d\n}", this->filename.c_str(), this->texID, this->width, this->height);
	return TEX(*this);
}

TEX TEX::Builder::buildTextureAtlasArray(int rows, int cols) {
	int channels = this->components == GL_RGB ? 3 : 4;

	int img_channels;
	unsigned char *image = stbi_load(this->filename.c_str(), &this->width, &this->height, &img_channels, channels);

	size_t size = this->width * this->height * channels * sizeof(unsigned char);
	unsigned char *imageArray = (unsigned char *)malloc(size);

	int sprite_width = width / cols;
	int sprite_height = height / rows;
	int imageArrayIndex = 0;
	for (int spriteIndex = 0; spriteIndex < rows*cols; spriteIndex++) {
		int r = spriteIndex / cols;
		int c = spriteIndex % cols;
		for (int spritePixel = 0; spritePixel < sprite_width * sprite_height; spritePixel++) {
			int u = spritePixel % sprite_width;
			int v = spritePixel / sprite_width;


			int offset = (c * sprite_width + r * sprite_width * sprite_height * cols);
			int index = (offset + (u + v * width))*channels;

			for (int i = 0; i < channels; i++) {
				imageArray[imageArrayIndex] = image[index+i];
				imageArrayIndex++;
			}
		}
	}

	stbi_image_free(image);

	int num_sprites = rows * cols;
	int mipmapLevelCount = 4;

	this->textureTarget = GL_TEXTURE_2D_ARRAY;
	glGenTextures(1, &this->texID);
	glBindTexture(this->textureTarget, this->texID);
	glTexStorage3D(this->textureTarget, mipmapLevelCount, storage, sprite_width, sprite_height, num_sprites);
	glTexSubImage3D(this->textureTarget, 0, 0, 0, 0, sprite_width, sprite_height, num_sprites, components, GL_UNSIGNED_BYTE, imageArray);

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

	glTexParameteri(this->textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(this->textureTarget, GL_TEXTURE_MAG_FILTER, filter);

	glBindTexture(this->textureTarget, 0);

	free(imageArray);

	return TEX(*this);
}