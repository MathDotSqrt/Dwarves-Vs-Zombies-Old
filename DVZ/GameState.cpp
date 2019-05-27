#include "GameState.h"

GameState::~GameState() {

}

bool GameState::isInitalized() {
	return this->m_isInitalized;
}

unordered_map<std::type_index, GameState*> GameState::gameStateInstances;