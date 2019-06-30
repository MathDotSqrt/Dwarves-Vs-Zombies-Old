#include "OpenGLRenderer.h"
#include "Window.h"
#include "Shader.h"
#include "preamble.glsl"
#include "glm.hpp"
#include <gtx/transform.hpp>
#include <gtx/quaternion.hpp>

using namespace Graphics;

OpenGLRenderer::OpenGLRenderer() {
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init(Scene *scene) {
	this->scene = scene;
	
	Camera camera = {
		glm::vec3(0, 0, 0),
		glm::vec3(0, 0, -1),
		glm::vec3(0, 1, 0),
		70,
		(double)Window::getWidth() / Window::getHeight(),
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
	Camera *camera = &scene->cameraCache[scene->getMainCameraID()];
	camera->aspect = (float)newWidth / newHeight;

	this->perspectiveProjection = glm::perspective(camera->fov, camera->aspect, camera->near, camera->far);

	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::prerender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void OpenGLRenderer::render() {
	glm::mat4 ident = glm::identity<glm::mat4>();
	
	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
	shader->use();

	Camera *camera = &scene->cameraCache[scene->getMainCameraID()];

	glm::mat4 view = glm::lookAt(camera->eye, camera->eye + camera->target, camera->up);

	for (auto instanceID : scene->instanceCache) {
		Instance *instance = &scene->instanceCache[instanceID];

		Mesh *mesh = &scene->meshCache[instance->meshID];
		Transformation *transformation = &scene->transformationCache[instance->transformationID];
		Material *material = &scene->materialCache[mesh->materialID];


		glm::quat rot(transformation->rotation);
		glm::vec3 axis = glm::axis(rot);

		float angle = glm::angle(rot);

		glm::mat4 model;

		model = glm::translate(ident, transformation->position);
		model = glm::rotate(model, angle, axis);
		model = glm::scale(model, transformation->scale);

		shader->setUniformMat4("model", perspectiveProjection * view * model);
		shader->setUniform3f("color", material->color);

		mesh->model.getVAO().bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);

		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}
