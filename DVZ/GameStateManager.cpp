#include "GameStateManager.h"
#include "GameState.h"

GameStateManager::GameStateManager() {
	vector<GameState*> currentStates;
	this->gameStateStack.push_back(currentStates);

}


GameStateManager::~GameStateManager() {
}

void GameStateManager::initState(GameState *state) {
	if (!state->isInitalized()) {
		state->init();
	}
}

void GameStateManager::cleanUpState(GameState *state) {
	if (state->isInitalized()) {
		state->cleanUp();
	}
}

void GameStateManager::stackState(GameState *state) {
	GameState *topState = this->getTopGameState();
	topState->obscuring();

	this->gameStateStack.back().push_back(state);

	initState(state);
	state->entered();
}

void GameStateManager::enterState(GameState *state) {
	vector<GameState*> currentStates = this->getCurrentGameStates();
	for (GameState* gs : currentStates) {
		gs->leaving();
	}

	vector<GameState*> concurrentStateStack;
	concurrentStateStack.push_back(state);
	this->gameStateStack.push_back(concurrentStateStack);

	initState(state);
	state->entered();
}

void GameStateManager::popCurrentState() {
	GameState *topState = this->getTopGameState();
	topState->leaving();
	
	vector<GameState*> *currentStates = &this->gameStateStack.back();
	currentStates->pop_back();

	if (currentStates->size() > 0) {
		currentStates->back()->revealing();
	}
	else {
		this->gameStateStack.pop_back();
		this->gameStateStack.back().back()->entered();
	}
}

void GameStateManager::update(float delta) {
	vector<GameState*> currentStates = this->getCurrentGameStates();

	for (GameState *state : currentStates) {
		state->update(delta);
	}
}

GameState* GameStateManager::getTopGameState() {
	return this->gameStateStack.back().back();
}

vector<GameState*> GameStateManager::getCurrentGameStates() {
	return this->gameStateStack.back();
}