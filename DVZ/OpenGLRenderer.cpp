#include "OpenGLRenderer.h"
#include "Shader.h"
#include "preamble.glsl"
#include <GLFW/glfw3.h>
#include "Window.h"
#include "glm.hpp"
#include "gtx/transform.hpp"
#include "gtx/quaternion.hpp"

using namespace Graphics;

void OpenGLRenderer::init() {
	if (glewInit() != GLEW_OK) {
		LOG_ERROR("Failed to init GLEW");

		exit(-2);
	}

	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);

	Window::addResizeCallback(resize);
	resize(Window::getWidth(), Window::getHeight());
}

void OpenGLRenderer::resize(int newWidth, int newHeight) {
	LOG_INFO("Resize (%d, %d)", newWidth, newHeight);
	glViewport(0, 0, newWidth, newHeight);
}

void OpenGLRenderer::preRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::render(Scene *scene) {
	glm::mat4 ident = glm::identity<glm::mat4>();
	
	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
	shader->use();

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

		shader->setUniformMat4("model", model);
		shader->setUniform3f("color", material->color);

		mesh->model.getVAO().bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);

		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}
