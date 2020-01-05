#pragma once
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include "entt.hpp"
#include "GameStateManager.h"
#include "macrologger.h"

class GameState {
	friend class GameStateManager;

private:
	static std::unordered_map<std::type_index, GameState*> gameStateInstances;
	static void deleteAllInstances();

protected:
	GameStateManager *gsm;

	bool m_isInitalized = false;

public:
	//DO NOT USE
	GameState(GameStateManager *gsm);
	virtual ~GameState() = 0;

	template<class T>
	static GameState* getInstance(GameStateManager *gsm) {
		GameState *state = gameStateInstances[typeid(T)];
		if (state == nullptr) {
			state = new T(gsm);
			gameStateInstances[typeid(T)] = state;
		}

		return state;
	}

	template<class T>
	static void deleteInstance() {
		GameState *state = gameStateInstances[typeid(T)];

		if (state != nullptr) {
			LOG_INFO("Delete state");
			delete state;
			gameStateInstances.erase(typeid(T));
		}
	}

	virtual void init() = 0;
	virtual void cleanUp() = 0;
	
	virtual void entered() = 0;
	virtual void leaving() = 0;

	virtual void update(float delta) = 0;

	bool isInitalized();
};

