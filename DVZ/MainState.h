#pragma once
#include "GameState.h"
#include "entt.hpp"
#include "SystemManager.h"
class MainState : public GameState{
private:
	entt::registry engine;
	SystemManager systemManager;

public:
	MainState(GameStateManager *gsm);
	~MainState();

	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

