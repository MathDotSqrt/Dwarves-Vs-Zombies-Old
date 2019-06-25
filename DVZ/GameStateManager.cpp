#include "GameStateManager.h"
#include "GameState.h"
#include "GameState.h"

GameStateManager::GameStateManager(){
	
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

void GameStateManager::enterState(GameState *state) {
	if (this->gameStateStack.size() > 0) {
		this->gameStateStack.back()->leaving();
	}

	this->gameStateStack.push_back(state);

	initState(state);
	state->entered();
}

void GameStateManager::popCurrentState() {
	if (this->gameStateStack.size() > 0) {
		this->gameStateStack.back()->leaving();
		this->gameStateStack.pop_back();
	}

	if (this->gameStateStack.size() > 0) {
		this->gameStateStack.back()->entered();
	}
}

void GameStateManager::update(float delta) {
	this->gameStateStack.back()->update(delta);
}

void GameStateManager::disposeAllStates() {
	GameState::deleteAllInstances();
	this->gameStateStack.clear();
}