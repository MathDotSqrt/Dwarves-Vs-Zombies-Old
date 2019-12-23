#include "ColorDepthFBO.h"
#include "GL/glew.h"

using namespace Graphics;

ColorDepthFBO::ColorDepthFBO(int width, int height) :
	FBO::FBO(width, height),
	color(TEX::Builder(width, height).rgb().clampToEdge().linear().buildTexture()),
	depth(TEX::Builder(width, height).depth24().clampToEdge().linear().buildTexture()) {
	
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.getTexID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.getTexID(), 0);
	unbind();
}

ColorDepthFBO::ColorDepthFBO(ColorDepthFBO &&other) : 
	color(std::move(other.color)), 
	depth(std::move(other.depth)), 
	FBO::FBO(std::move(other)) {
	other.fboID = 0;
}

ColorDepthFBO::~ColorDepthFBO() {
	dispose();
}

ColorDepthFBO& ColorDepthFBO::operator&=(ColorDepthFBO &&other) {
	if (this != &other) {
		dispose();
		std::swap(fboID, other.fboID);
		std::swap(color, other.color);
		std::swap(depth, other.depth);
	}

	return *this;
}

TEX& ColorDepthFBO::getColorAttachment() {
	return color;
}

TEX& ColorDepthFBO::getDepthAttachment() {
	return depth;
}
