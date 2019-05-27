#pragma once
#include <vector>


class GameState;
using namespace std;

class GameStateManager {
private:
	vector<vector<GameState*>> gameStateStack;

	GameState* getTopGameState();
	vector<GameState*> getCurrentGameStates();

public:
	GameStateManager();
	~GameStateManager();

	void initState(GameState *state);
	void cleanUpState(GameState *state);

	void stackState(GameState *state);
	void enterState(GameState *state);

	void popCurrentState();
	void disposeCurrentState();

	void disposeAllStates();

	void update();
};

