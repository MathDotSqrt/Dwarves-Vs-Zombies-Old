#include "OpenGLRenderer.h"
#include "render_functions.h"
#include "Scene.h"
#include "Window.h"
#include "ShaderSet.h"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include "ChunkRenderDataManager.h"
#include "Timer.h"
#include "QuadGeometry.h"
using namespace Graphics;

MaterialID ColorMaterial::type = MaterialID::COLOR_MATERIAL_ID;
MaterialID NormalMaterial::type = MaterialID::NORMAL_MAERIAL_ID;
MaterialID BasicLitMaterial::type = MaterialID::BASIC_LIT_MATERIAL_ID;
MaterialID TextureMaterial::type = MaterialID::TEXTURE_MATERIAL_ID;
MaterialID BlockMaterial::type = MaterialID::BLOCK_MATERIAL_ID;

OpenGLRenderer::OpenGLRenderer(Scene *scene) : 
	final(Window::getWidth(), Window::getHeight()), 
	shadow(2048, 2048),
	vbo(GL_ARRAY_BUFFER),
	start(Window::getTime())
	{
	init(scene);
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init(Scene *scene) {
	LOG_RENDER("Init");
	this->scene = scene;
	
	glClearColor(.4f, .4f, .4f, 1);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	GLbyte quad_array[]{
		-1, -1,
		1, -1,
		-1,  1,
		-1,  1,
		1, -1,
		1,  1,
	};
	quad.bind();
	vbo.bind();
	vbo.bufferData(sizeof(quad_array), quad_array, GL_STATIC_DRAW);
	quad.bufferInterleavedData(Attrib<POSITION_ATTRIB_LOCATION, glm::i8vec2>());
	vbo.unbind();
	quad.unbind();

	Scene::Camera camera = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0),
		70 * 3.14159f / 180.0f,
		(float)Window::getWidth() / Window::getHeight(),
		.1f,
		1000
	};

	unsigned int cameraID = this->scene->createCameraInstance(camera);
	this->scene->setMainCamera(cameraID);

	Window::addResizeCallback(this);
	resize(Window::getWidth(), Window::getHeight());
	window_width = Window::getWidth();
	window_height = Window::getHeight();
}

void OpenGLRenderer::resize(int newWidth, int newHeight) {
	LOG_RENDER("Resize (%d, %d)", newWidth, newHeight);
	Scene::Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	camera->aspect = (float)newWidth / newHeight;

	this->perspectiveProjection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);
	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::prerender() {
	duration = Window::getTime() - start;

	sortedRenderStateKeys.clear();
	
	RenderStateKey chunk_shadow_render(ViewPort::SHADOW, ViewPortLayer::DEFAULT, BlendType::OPAQUE, MaterialID::CHUNK_SHADOW_MATERIAL_ID, 69);
	RenderStateKey chunk_render(ViewPort::FINAL, ViewPortLayer::DEFAULT, BlendType::OPAQUE, MaterialID::CHUNK_MATERIAL_ID, 69);
	sortedRenderStateKeys.push_back(chunk_shadow_render);
	sortedRenderStateKeys.push_back(chunk_render);

	for (unsigned int instanceID : scene->instanceCache) {
		Scene::Instance &instance = scene->instanceCache[instanceID];
		MaterialID matID = instance.materialType;

		RenderStateKey key(ViewPort::FINAL, ViewPortLayer::DEFAULT, BlendType::OPAQUE, matID, instanceID);
		sortedRenderStateKeys.push_back(key);
	}

	

	std::sort(sortedRenderStateKeys.begin(), sortedRenderStateKeys.end());
}

void OpenGLRenderer::render(Voxel::ChunkRenderDataManager *manager) {
	currentPort = ViewPort::NUM_VIEW_PORTS;
	ShaderVariables::shader_time = getShaderTime();

	std::vector<RenderStateKey>::const_iterator start = sortedRenderStateKeys.begin();
	std::vector<RenderStateKey>::const_iterator end = sortedRenderStateKeys.end();

	while (start != end) {
		swapViewPorts(*start);

		switch (start->getMaterialID()) {
		case MaterialID::COLOR_MATERIAL_ID:
			start = render_basic(scene, start, end);
			break;
		case MaterialID::NORMAL_MAERIAL_ID:
			start = render_normal(scene, start, end);
			break;
		case MaterialID::BASIC_LIT_MATERIAL_ID:
			start = render_basic_lit(scene, start, end);
			break;
		case MaterialID::CHUNK_MATERIAL_ID:
			start = render_chunks(manager, scene, &shadow, start, end);
			break;
		case MaterialID::CHUNK_SHADOW_MATERIAL_ID:
			start = render_chunks_shadow(manager, scene, start, end);
			break;
		default:
			start++;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	renderPostProcess();
}

void OpenGLRenderer::postrender() {
	
}

void OpenGLRenderer::swapViewPorts(RenderStateKey key) {
	ViewPort newPort = key.getViewPort();

	if (newPort == currentPort) {
		return;
	}

	currentPort = newPort;

	switch (currentPort) {
	case ViewPort::SHADOW:
		bindShadowPort();
		break;
	case ViewPort::FINAL:
	default:
		bindFinalPort();
		break;
	}

}

void OpenGLRenderer::bindShadowPort() {
	shadow.bind();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	auto camera = &scene->cameraCache[scene->getSunCameraID()];
	ShaderVariables::camera_pos = camera->eye;
	ShaderVariables::p = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, .1f, 100.0f);
	ShaderVariables::v = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	ShaderVariables::vp = ShaderVariables::p * ShaderVariables::v;

}

void OpenGLRenderer::bindFinalPort() {
	final.bind();
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto camera = &scene->cameraCache[scene->getMainCameraID()];
	ShaderVariables::camera_pos = camera->eye;
	ShaderVariables::sunVP = ShaderVariables::vp;			//this is the vp from the previous view port 
	ShaderVariables::p = perspectiveProjection;
	ShaderVariables::v = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	ShaderVariables::vp = ShaderVariables::p * ShaderVariables::v;
}

void OpenGLRenderer::renderPostProcess() {
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, window_width, window_height);

	Shader::GLSLProgram *program = Shader::getShaderSet({"res/frame_buffer_shader.vert", "res/frame_buffer_shader.frag"});
	program->use();

	//shadow.getDepthAttachment().bindActiveTexture(0);
	final.getColorAttachment().bindActiveTexture(0);
	program->setUniform1f("time", getShaderTime());
	quad.bind();
	glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	quad.unbind();

	program->end();

}

float OpenGLRenderer::getShaderTime() {
	return (float)duration;
}