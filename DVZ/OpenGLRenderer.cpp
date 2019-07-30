#include "OpenGLRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "preamble.glsl"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

using namespace Graphics;

MaterialID ColorMaterial::type = MaterialID::COLOR_MATERIAL_ID;
MaterialID NormalMaterial::type = MaterialID::NORMAL_MAERIAL_ID;
MaterialID TextureMaterial::type = MaterialID::TEXTURE_MATERIAL_ID;

OpenGLRenderer::OpenGLRenderer() {
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init(Scene *scene) {
	LOG_RENDER("Init");
	this->scene = scene;
	
	Camera camera = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0),
		70,
		(float)Window::getWidth() / Window::getHeight(),
		.1f,
		1000
	};

	unsigned int cameraID = this->scene->createCameraInstance(camera);
	this->scene->setMainCamera(cameraID);

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	Window::addResizeCallback(this);
	this->resize(Window::getWidth(), Window::getHeight());
}

void OpenGLRenderer::resize(int newWidth, int newHeight) {
	LOG_RENDER("Resize (%d, %d)", newWidth, newHeight);
	Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	camera->aspect = (float)newWidth / newHeight;

	this->perspectiveProjection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);

	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::prerender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_CW);

	this->sortedRenderStateKeys.clear();

	for (unsigned int instanceID : scene->instanceCache) {
		RenderStateKey key = 0L;
		MaterialID matID = MaterialID::NONE_MATERIAL_ID;

		Instance *instance = &scene->instanceCache[instanceID];
		Mesh *mesh = &scene->meshCache[instance->meshID];
		matID = mesh->typeID;

		RenderState state;
		state.instanceID = instanceID;
		state.materialID = matID;

		key = this->createRenderStateKey(state);
		sortedRenderStateKeys.push_back(key);

	}
	std::sort(this->sortedRenderStateKeys.begin(), this->sortedRenderStateKeys.end());

}

void OpenGLRenderer::render() {
	Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	glm::mat4 view = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);
	glm::mat4 vp = perspectiveProjection * view;

	int index = 0;
	index = renderBasic(index, vp);

	index = renderNormal(index, vp);

	//glBindVertexArray(0);
}

int OpenGLRenderer::renderBasic(int startIndex, glm::mat4 vp) {
	if (!this->isValidState(startIndex, MaterialID::COLOR_MATERIAL_ID)) {
		return startIndex;
	}

	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
	shader->use();
	int index = startIndex;
	do {
		RenderState state = this->getRenderStateFromIndex(index);

		Instance *instance = &scene->instanceCache[state.instanceID];
		Mesh *mesh = &scene->meshCache[instance->meshID];
		Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		shader->setUniformMat4("model", vp * model);
		
		ColorMaterial *material = &scene->colorMaterialCache[mesh->materialInstanceID];
		shader->setUniform3f("color", material->color);

		mesh->model.getVAO().bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::COLOR_MATERIAL_ID));

	shader->end();
	glBindVertexArray(0);

	return index;
}

int OpenGLRenderer::renderNormal(int startIndex, glm::mat4 vp) {
	if (!this->isValidState(startIndex, MaterialID::NORMAL_MAERIAL_ID)) {
		return startIndex;
	}

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 ident = glm::identity<glm::mat4>();
	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
	shader->use();

	int index = startIndex;
	do {
		RenderState state = this->getRenderStateFromIndex(index);

		Instance *instance = &scene->instanceCache[state.instanceID];
		Mesh *mesh = &scene->meshCache[instance->meshID];
		Transformation *transformation = &scene->transformationCache[instance->transformationID];

		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;
		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		shader->setUniformMat4("model", vp * model);
		float color[3] = { 1, .5f, .5f };
		shader->setUniform3f("color", color);

		mesh->model.getVAO().bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);

		index++;
	} while (this->isValidState(index, MaterialID::NORMAL_MAERIAL_ID));

	shader->end();
	glBindVertexArray(0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	return index;
}

//int OpenGLRenderer::renderBasic(int startKeyIndex, glm::mat4 vp) {
//	glm::mat4 ident = glm::identity<glm::mat4>();
//	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
//	shader->use();
//	for (auto instanceID : scene->instanceCache) {
//		Instance *instance = &scene->instanceCache[instanceID];
//
//		ColorMesh *mesh = &scene->meshCache[instance->meshID];
//		Transformation *transformation = &scene->transformationCache[instance->transformationID];
//
//		glm::quat rot(transformation->rotation);
//		glm::vec3 axis = glm::axis(rot);
//
//		float angle = glm::angle(rot);
//
//		glm::mat4 model;
//		model = glm::translate(ident, transformation->position);
//		model = glm::rotate(model, angle, axis);
//		model = glm::scale(model, transformation->scale);
//
//		shader->setUniformMat4("model", vp * model);
//		
//		ColorMaterial *material = &scene->colorMaterialCache[mesh->materialID];
//		shader->setUniform3f("color", material->color);
//
//		mesh->model.getVAO().bind();
//		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);
//		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);
//	}
//
//	
//}

bool OpenGLRenderer::isValidState(int sortedStateKeyIndex, MaterialID typeID) {
	return sortedStateKeyIndex < sortedRenderStateKeys.size()
		&& typeID == getRenderStateFromKey(sortedRenderStateKeys[sortedStateKeyIndex]).materialID;
}

OpenGLRenderer::RenderStateKey OpenGLRenderer::createRenderStateKey(OpenGLRenderer::RenderState state) {
	RenderStateKey key = 0L;

	key |= state.instanceID;
	key |= ((unsigned long long)state.materialID) << sizeof(state.instanceID) * 8;

	return key;
}

OpenGLRenderer::RenderState OpenGLRenderer::getRenderStateFromKey(RenderStateKey key) {
	RenderState state;

	state.instanceID = key & 0xffffffff;
	state.materialID = (MaterialID)((key >> sizeof(state.instanceID) * 8) & 0xff);

	return state;
}

OpenGLRenderer::RenderState OpenGLRenderer::getRenderStateFromIndex(int sortedRenderStateKeyIndex) {
	return getRenderStateFromKey(sortedRenderStateKeys[sortedRenderStateKeyIndex]);
}