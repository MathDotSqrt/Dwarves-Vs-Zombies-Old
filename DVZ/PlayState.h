#pragma once
#include "GameState.h"
#include "GameStateManager.h"
#include "entt.hpp"
#include "SystemManager.h"
#include "Scene.h"
#include "OpenGLRenderer.h"

class PlayState : public GameState {
private:
	entt::registry engine;
	SystemManager manager;

	Graphics::Scene scene;
	Graphics::OpenGLRenderer renderer;

public:
	PlayState(GameStateManager *gsm);
	~PlayState();

	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

