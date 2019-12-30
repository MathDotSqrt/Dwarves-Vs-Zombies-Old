#pragma once
#include "Engine.h"
#include "GameState.h"
#include "GameStateManager.h"

class PlayState : public GameState {
private:
	Engine e;
	entt::registry engine;
	/*Graphics::Scene scene;
	Graphics::OpenGLRenderer renderer;*/

public:
	PlayState(GameStateManager *gsm);
	~PlayState();

	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

