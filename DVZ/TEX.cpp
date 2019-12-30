#include "TEX.h"

using namespace Graphics;

TEX::TEX(TEX &&other) : textureTarget(other.textureTarget), width(other.width), height(other.height){
	texID = other.texID;
	other.dispose();
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
		std::swap(textureTarget, other.textureTarget);
		std::swap(width, other.width);
		std::swap(height, other.height);
		std::swap(texID, other.texID);
	}

	return *this;
}

void TEX::bind() {
	glBindTexture(textureTarget, texID);
}

void TEX::bindActiveTexture(int unit) {
	if (unit >= 0) {
		glActiveTexture(GL_TEXTURE0 + unit);
		bind();
	}
}

void TEX::unbind() {
	glBindTexture(textureTarget, 0);
}

void TEX::dispose() {
	if (texID) {
		LOG_INFO("Deleting texture: %d", texID);
		glDeleteTextures(1, &texID);
		texID = 0;
	}
}

GLuint TEX::getTexID() {
	return texID;
}

TEX::Builder::Builder(int width, int height) {
	this->width = width;
	this->height = height;
	filename = "";
	mipmap = false;
	useMipMap = false;
	rgb().repeat().nearest().borderColor(0, 0, 0, 1).unsignedByteType();
}

TEX::Builder::Builder(std::string filename) {
	this->filename = filename;
	mipmap = false;
	useMipMap = false;
	rgb().repeat().nearest().borderColor(0, 0, 0, 1).unsignedByteType();
}

TEX::Builder::~Builder() {

}

TEX::Builder& TEX::Builder::rgb() {
	components = GL_RGB;
	storage = GL_RGB8;
	return *this;
}

TEX::Builder& TEX::Builder::rgba() {
	components = GL_RGBA;
	storage = GL_RGBA8;
	return *this;
}

TEX::Builder& TEX::Builder::depth24() {
	components = GL_DEPTH_COMPONENT;
	storage = GL_DEPTH_COMPONENT24;
	return *this;
}

TEX::Builder& TEX::Builder::unsignedByteType() {
	dataType = GL_UNSIGNED_BYTE;
	return *this;
}

TEX::Builder& TEX::Builder::floatType() {
	dataType = GL_FLOAT;
	return *this;
}

TEX::Builder& TEX::Builder::repeat() {
	wrapS = GL_REPEAT;
	wrapT = GL_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::mirrorRepeat() {
	wrapS = GL_MIRRORED_REPEAT;
	wrapT = GL_MIRRORED_REPEAT;
	return *this;
}

TEX::Builder& TEX::Builder::clampToEdge() {
	wrapS = GL_CLAMP_TO_EDGE;
	wrapT = GL_CLAMP_TO_EDGE;
	return *this;
}

TEX::Builder& TEX::Builder::clampToBorder() {
	wrapS = GL_CLAMP_TO_BORDER;
	wrapT = GL_CLAMP_TO_BORDER;
	return *this;
}

TEX::Builder& TEX::Builder::borderColor(float r, float g, float b, float a) {
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
	return *this;
}

TEX::Builder& TEX::Builder::nearest() {
	filter = GL_NEAREST;
	return *this;
}

TEX::Builder& TEX::Builder::linear() {
	filter = GL_LINEAR;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapNearest() {
	mipmap = GL_LINEAR;
	useMipMap = true;
	return *this;
}

TEX::Builder& TEX::Builder::mipmapLinear() {
	mipmap = GL_LINEAR;
	useMipMap = true;
	return *this;
}

TEX TEX::Builder::buildTexture() {
	unsigned char *image = nullptr;
	if (filename != "") {
		int channels;
		unsigned char *image = stbi_load(filename.c_str(), &width, &height, &channels, 3);
	}


	textureTarget = GL_TEXTURE_2D;
	
	int mipmapLevelCount = 0;


	glGenTextures(1, &texID);
	glBindTexture(textureTarget, texID);
	glTexImage2D(textureTarget, mipmapLevelCount, storage, width, height, 0, components, dataType, image);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, color);

	GLenum f;
	
	if (useMipMap) {
		glGenerateMipmap(textureTarget);
		unsigned char bit1 = mipmap == GL_LINEAR;
		unsigned char bit2 = filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
			case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
			case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
			case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
			case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = filter;
	}

	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);

	stbi_image_free(image);

	glBindTexture(textureTarget, 0);

	LOG_INFO("Creating Texture:{\n\tname: %s, texture: %d, width: %d, height: %d\n}", filename.c_str(), texID, width, height);
	return TEX(*this);
}

TEX TEX::Builder::buildTextureAtlasArray(int rows, int cols) {
	int channels = components == GL_RGB ? 3 : 4;

	int img_channels;
	unsigned char *image = stbi_load(filename.c_str(), &width, &height, &img_channels, channels);

	size_t size = width * height * channels * sizeof(unsigned char);
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
	int mipmapLevelCount = 8;

	textureTarget = GL_TEXTURE_2D_ARRAY;
	glGenTextures(1, &texID);
	glBindTexture(textureTarget, texID);
	glTexStorage3D(textureTarget, mipmapLevelCount, storage, sprite_width, sprite_height, num_sprites);
	glTexSubImage3D(textureTarget, 0, 0, 0, 0, sprite_width, sprite_height, num_sprites, components, dataType, imageArray);

	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, wrapT);
	glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, color);
	


	GLenum f;

	if (useMipMap) {
		glGenerateMipmap(textureTarget);
		unsigned char bit1 = mipmap == GL_LINEAR;
		unsigned char bit2 = filter == GL_LINEAR;
		switch ((bit2 << 1) | bit1) {
		case 0x0: f = GL_NEAREST_MIPMAP_NEAREST; break;
		case 0x1: f = GL_LINEAR_MIPMAP_NEAREST; break;
		case 0x2: f = GL_NEAREST_MIPMAP_LINEAR; break;
		case 0x3: f = GL_LINEAR_MIPMAP_LINEAR; break;
		}
	}
	else {
		f = filter;
	}

	glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, f);
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, filter);

	glBindTexture(textureTarget, 0);

	free(imageArray);

	return TEX(*this);
}