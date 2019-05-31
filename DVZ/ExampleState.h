#pragma once
#include "GameState.h"
class ExampleState : public GameState {
public:
	ExampleState(GameStateManager *gsm) : GameState(gsm) {}
	~ExampleState();

	virtual void init() override;
	virtual void cleanUp() override;

	virtual void entered() override;
	virtual void leaving() override;

	virtual void obscuring() override;
	virtual void revealing() override;

	virtual void update(float delta) override;
};

