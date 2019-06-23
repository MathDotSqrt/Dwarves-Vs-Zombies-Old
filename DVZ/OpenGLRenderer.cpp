#include "OpenGLRenderer.h"
#include "Shader.h"
#include "preamble.glsl"
#include <GLFW/glfw3.h>

OpenGLRenderer::OpenGLRenderer() {
}


OpenGLRenderer::~OpenGLRenderer() {
}

void OpenGLRenderer::init() {
}

void OpenGLRenderer::resize(int width, int height) {

}

void OpenGLRenderer::render(Scene *scene) {


	Shader::GLSLShader *shader = Shader::getShader("basic_shader");
	shader->use();

	for (auto instanceID : scene->instanceCache) {
		Instance *instance = &scene->instanceCache[instanceID];

		Mesh *mesh = &scene->meshCache[instance->meshID];
		Transformation *transformation = &scene->transformationCache[instance->transformationID];
		Material *material = &scene->materialCache[mesh->materialID];

		shader->setUniform3f("position", transformation->position);
		shader->setUniform3f("scale", transformation->scale);
		shader->setUniform3f("color", material->color);

		mesh->model.getVAO().bind();
		glEnableVertexAttribArray(POSITION_ATTRIB_LOCATION);

		glDrawElements(GL_TRIANGLES, mesh->model.getVertexCount(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
}
