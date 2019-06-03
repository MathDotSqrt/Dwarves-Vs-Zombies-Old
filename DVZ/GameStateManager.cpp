#include "GameStateManager.h"
#include "GameState.h"

GameStateManager::GameStateManager(entt::registry &engine) : engine(engine){
	vector<GameState*> currentStates;
	this->gameStateStack.push_back(currentStates);
}


GameStateManager::~GameStateManager() {

}

void GameStateManager::initState(GameState *state) {
	if (!state->isInitalized()) {
		state->init(this->engine);
	}
}

void GameStateManager::cleanUpState(GameState *state) {
	if (state->isInitalized()) {
		state->cleanUp(this->engine);
	}
}

void GameStateManager::stackState(GameState *state) {
	GameState *topState = this->getTopGameState();
	topState->obscuring(this->engine);

	this->gameStateStack.back().push_back(state);

	initState(state);
	state->entered(this->engine);
}

void GameStateManager::enterState(GameState *state) {
	vector<GameState*> currentStates = this->getCurrentGameStates();
	for (GameState* gs : currentStates) {
		gs->leaving(this->engine);
	}

	vector<GameState*> concurrentStateStack;
	concurrentStateStack.push_back(state);
	this->gameStateStack.push_back(concurrentStateStack);

	initState(state);
	state->entered(this->engine);
}

void GameStateManager::popCurrentState() {
	GameState *topState = this->getTopGameState();
	topState->leaving(this->engine);
	
	vector<GameState*> &currentStates = this->gameStateStack.back();
	currentStates.pop_back();

	if (currentStates.size() > 0) {
		currentStates.back()->revealing(this->engine);
	}
	else {
		this->gameStateStack.pop_back();
		this->gameStateStack.back().back()->entered(this->engine);
	}
}

void GameStateManager::update(float delta) {
	vector<GameState*> currentStates = this->getCurrentGameStates();
	//todo make this a reference for stacked states to update instantly 
	for (GameState *state : currentStates) {
		state->update(this->engine, delta);
	}
}

GameState* GameStateManager::getTopGameState() {
	return this->gameStateStack.back().back();
}

vector<GameState*> GameStateManager::getCurrentGameStates() {
	return this->gameStateStack.back();
}

void GameStateManager::disposeAllStates() {
	GameState::deleteAllInstances();
	this->gameStateStack.clear();
}