#pragma once
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include "GameStateManager.h"
#include "macrologger.h"

class GameState {
private:
	static unordered_map<std::type_index, GameState*> gameStateInstances;

protected:
	GameStateManager *gsm;
	bool m_isInitalized = false;

public:
	GameState(GameStateManager *gsm) : gsm(gsm) {}
	~GameState();

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

	virtual void obscuring() = 0;
	virtual void revealing() = 0;

	virtual void update() = 0;

	bool isInitalized();
};

