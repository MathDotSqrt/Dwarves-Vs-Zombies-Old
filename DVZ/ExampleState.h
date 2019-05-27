#pragma once
#include "GameState.h"
class ExampleState : public GameState {
public:
	ExampleState(GameStateManager *gsm) : GameState(gsm) {}
	~ExampleState();

	virtual void init();
	virtual void cleanUp();

	virtual void entered();
	virtual void leaving();

	virtual void obscuring();
	virtual void revealing();

	virtual void update();
};

