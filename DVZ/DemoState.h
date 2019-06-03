#pragma once
#include "GameState.h"
#include <GL/glew.h>
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "TEX.h"


class DemoState : public GameState {
private:
	Shader::GLSLShader *shader;
	VAO *vao;
	VBO *vbo;
	VBO *ebo;
	TEX *texture1;
	TEX *texture2;

public:
	DemoState(GameStateManager *gsm) : GameState(gsm) {}
	~DemoState();

	// Inherited via GameState
	virtual void init(entt::registry &engine) override;
	virtual void cleanUp(entt::registry &engine) override;
	virtual void entered(entt::registry &engine) override;
	virtual void leaving(entt::registry &engine) override;
	virtual void obscuring(entt::registry &engine) override;
	virtual void revealing(entt::registry &engine) override;
	virtual void update(entt::registry &engine, float delta) override;
};

