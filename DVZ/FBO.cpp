#include "FBO.h"
#include "GL/glew.h"
#include <utility>
#include "Window.h"
using namespace Graphics;

FBO::FBO(int width, int height) : 
	color(TEX::Builder(width, height).rgb().clampToEdge().linear().buildTexture()), 
	depth(TEX::Builder(width, height).depth24().clampToEdge().linear().buildTexture()) {

	glGenFramebuffers(1, &fboID);

	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color.getTexID(), 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.getTexID(), 0);
	unbind();
}

FBO::FBO(FBO &&other) : 
	fboID(other.fboID), 
	color(std::move(other.color)), 
	depth(std::move(other.depth)){
	other.fboID = 0;
}

FBO::~FBO() {
	dispose();
}

FBO& FBO::operator&=(FBO && other) {
	if (this != &other) {
		dispose();
		std::swap(fboID, other.fboID);
	}
	
	return *this;
}

void FBO::dispose() {
	if (fboID) {
		glDeleteFramebuffers(1, &fboID);
		fboID = 0;
	}
}

void FBO::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	glViewport(0, 0, getWidth(), getHeight());
}

void FBO::unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Window::getWidth(), Window::getHeight());
}

TEX& FBO::getColorAttachment() {
	return color;
}

TEX& FBO::getDepthAttachment() {
	return depth;
}

int FBO::getWidth() {
	return color.width;
}

int FBO::getHeight() {
	return color.height;
}