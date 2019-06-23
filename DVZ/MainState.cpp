#include "MainState.h"
#include "macrologger.h"
#include "PlayState.h"
#include "BasicRenderSystem.h"
#include "Components.h"

#include "VAO.h"
#include "VBO.h"
#include "Shader.h"

MainState::MainState(GameStateManager *gsm) : GameState(gsm) {

}


MainState::~MainState() {

}

void MainState::init() {
	LOG_INFO("MainState: init()");

	//this->systemManager.addSystem(this->engine, new BasicRenderSystem(0));
}

void MainState::cleanUp() {
	LOG_INFO("MainState: cleanUp()");

	this->systemManager.deleteAllActiveSystems(this->engine);

}

void MainState::entered() {
	LOG_INFO("MainState: entered()");
	float vertices[] = { 0.0f, 0.5f, 0.5f, -0.5f, -0.5f, -0.5f }; 

	VAO model;
	model.bind();

	Shader::GLSLShader *shader = Shader::getShader("basic_shader");

	GLuint posAttrib = shader->getAttrib("vert_pos");

	VBO vert(GL_ARRAY_BUFFER);
	vert.bufferData(sizeof(vertices), vertices, GL_STATIC_DRAW);
	vert.bind();
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
	vert.unbind();
	model.unbind();

	
	entt::entity e = this->engine.create();
	engine.assign<BasicRenderComponent>(e, model, shader);

}

void MainState::leaving() {
	LOG_INFO("MainState: leaving()");

}

void MainState::update(float delta) {
	this->systemManager.updateSystems(this->engine, delta);
}