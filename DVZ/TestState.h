#pragma once
#include "GameState.h"

class TestState : public GameState
{
public:
	TestState(GameStateManager *gsm) : GameState(gsm) {}
	~TestState();

	virtual void init() override;
	virtual void cleanUp() override;

	virtual void entered() override;
	virtual void leaving() override;

	virtual void obscuring() override;
	virtual void revealing() override;

	virtual void update(float delta) override;
};

