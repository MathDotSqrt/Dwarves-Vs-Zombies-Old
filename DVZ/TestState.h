#pragma once
#include "GameState.h"

class TestState : public GameState
{
public:
	TestState(GameStateManager *gsm) : GameState(gsm) {}
	~TestState();

	virtual void init();
	virtual void cleanUp();

	virtual void entered();
	virtual void leaving();

	virtual void obscuring();
	virtual void revealing();

	virtual void update();
};

