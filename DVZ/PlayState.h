#pragma once
#include "GameState.h"
#include "GameStateManager.h"
class PlayState : public GameState
{
public:
	PlayState(GameStateManager *gsm);
	~PlayState();

	virtual void init() override;
	virtual void cleanUp() override;
	virtual void entered() override;
	virtual void leaving() override;
	virtual void update(float delta) override;
};

