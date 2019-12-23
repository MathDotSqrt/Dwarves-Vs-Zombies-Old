#include "DepthFBO.h"

using namespace Graphics;

DepthFBO::DepthFBO(int width, int height) : 
	FBO(width, height),
	depth(TEX::Builder(width, height).depth24().clampToEdge().nearest().buildTexture()){
	
	bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth.getTexID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	unbind();
}

DepthFBO::DepthFBO(DepthFBO &&other) : depth(std::move(other.depth)), FBO(std::move(other)){

}

DepthFBO::~DepthFBO() {
	dispose();
}

DepthFBO& DepthFBO::operator&=(DepthFBO &&other) {
	if( this != &other) {
		dispose();
		std::swap(fboID, other.fboID);
		std::swap(depth, other.depth);
	}

	return *this;
}

TEX& DepthFBO::getDepthAttachment() {
	return depth;
}