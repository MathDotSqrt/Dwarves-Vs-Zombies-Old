#pragma once
#include "GameState.h"
class ExampleState : public GameState {
public:
	ExampleState(GameStateManager *gsm) : GameState(gsm) {}
	~ExampleState();

	virtual void init(entt::registry &engine) override;
	virtual void cleanUp(entt::registry &engine) override;

	virtual void entered(entt::registry &engine) override;
	virtual void leaving(entt::registry &engine) override;

	virtual void obscuring(entt::registry &engine) override;
	virtual void revealing(entt::registry &engine) override;

	virtual void update(entt::registry &engine, float delta) override;
};

