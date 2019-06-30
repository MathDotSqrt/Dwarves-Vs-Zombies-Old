#pragma once
#include <vector>
#include <entt.hpp>

class GameState;
class ConcurrentGameState;

class GameStateManager {
private:
	std::vector<GameState*> gameStateStack;

public:
	GameStateManager();
	~GameStateManager();

	void initState(GameState *state);
	void cleanUpState(GameState *state);

	void enterState(GameState *state);

	template<typename STATE>
	void enterState() {
		this->enterState(GameState::getInstance<STATE>(this));
	}

	void popCurrentState();
	void disposeCurrentState();

	void disposeAllStates();

	void update(float delta);
};

