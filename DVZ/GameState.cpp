#include "GameState.h"

GameState::GameState(GameStateManager *gsm) : gsm(gsm) {
	//this->systemManager = new SystemManager();
}

GameState::~GameState() {
	//LOG_INFO("GAY");
}

bool GameState::isInitalized() {
	return this->m_isInitalized;
}

void GameState::deleteAllInstances() {
	LOG_INFO("Deleting all states...");
	for (auto it = gameStateInstances.begin(); it != gameStateInstances.end(); it++) {
		GameState *state = it->second;
		if (state != nullptr) {
			state->cleanUp();
			delete state;
			LOG_INFO("Deleted state");
		}
	}
	gameStateInstances.clear();
}

std::unordered_map<std::type_index, GameState*> GameState::gameStateInstances;