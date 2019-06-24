#pragma once
#include "GameState.h"
#include <GL/glew.h>
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "TEX.h"


class DemoState : public GameState {
private:
	Graphics::Shader::GLSLShader *shader;
	Graphics::VAO *vao;
	Graphics::VBO *vbo;
	Graphics::VBO *ebo;
	Graphics::TEX *texture1;
	Graphics::TEX *texture2;

public:
	DemoState(GameStateManager *gsm) : GameState(gsm) {}
	~DemoState();

	// Inherited via GameState
	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

