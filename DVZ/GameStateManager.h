#pragma once
#include <vector>
#include <entt.hpp>

class GameState;
using namespace std;

class GameStateManager {
private:
	entt::registry &engine;
	vector<vector<GameState*>> gameStateStack;

	GameState* getTopGameState();
	vector<GameState*> getCurrentGameStates();

public:
	GameStateManager(entt::registry &engine);
	~GameStateManager();

	void initState(GameState *state);
	void cleanUpState(GameState *state);

	void stackState(GameState *state);
	void enterState(GameState *state);

	template<typename STATE>
	void stackState() {
		this->stackState(GameState::getInstance<STATE>(this));
	}

	template<typename STATE>
	void enterState() {
		this->enterState(GameState::getInstance<STATE>(this));
	}

	void popCurrentState();
	void disposeCurrentState();

	void disposeAllStates();

	void update(float delta);
};

