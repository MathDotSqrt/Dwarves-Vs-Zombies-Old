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

OpenGLRenderer::OpenGLRenderer() : 
	final(Window::getWidth(), Window::getHeight()), 
	shadow(1024, 1024),
	vbo(GL_ARRAY_BUFFER),
	start(Window::getTime())
	{}


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
	quad.bufferInterleavedData(vbo, Attrib<POSITION_ATTRIB_LOCATION, glm::i8vec2>());
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

	RenderStateKey chunk_render(ViewPort::FINAL, ViewPortLayer::DEFAULT, BlendType::OPAQUE, MaterialID::CHUNK_MATERIAL_ID, 69);
	sortedRenderStateKeys.push_back(chunk_render);

	for (unsigned int instanceID : scene->instanceCache) {
		Scene::Instance instance = scene->instanceCache[instanceID];
		Scene::Mesh &mesh = scene->meshCache[instance.meshID];
		MaterialID matID = mesh.typeID;

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
			start = render_chunks(manager, scene, start, end);
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

	Scene::Camera *camera = nullptr;

	switch (currentPort) {
	case ViewPort::SHADOW:
		shadow.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera = &scene->cameraCache[scene->getSunCameraID()];
		ShaderVariables::camera_pos = camera->eye;
		ShaderVariables::p = ShaderVariables::identity;
		ShaderVariables::v = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
		ShaderVariables::vp = ShaderVariables::p * ShaderVariables::v;
		break;
	case ViewPort::FINAL:
	default:
		final.bind();
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		camera = &scene->cameraCache[scene->getMainCameraID()];
		ShaderVariables::camera_pos = camera->eye;
		ShaderVariables::p = perspectiveProjection;
		ShaderVariables::v = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
		ShaderVariables::vp = ShaderVariables::p * ShaderVariables::v;
		break;
	}

}

void OpenGLRenderer::renderPostProcess() {
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, window_width, window_height);

	Shader::GLSLProgram *program = Shader::getShaderSet({"frame_buffer_shader.vert", "frame_buffer_shader.frag"});
	program->use();

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