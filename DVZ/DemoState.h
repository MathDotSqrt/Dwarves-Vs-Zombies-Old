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
	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

