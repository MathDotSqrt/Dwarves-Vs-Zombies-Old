#pragma once
#include <unordered_map>
#include <typeindex>
#include <typeinfo>
#include <entt.hpp>
#include "GameStateManager.h"
#include "SystemManager.h"
#include "macrologger.h"

class GameState {
	friend class GameStateManager;

private:
	static unordered_map<std::type_index, GameState*> gameStateInstances;
	static void deleteAllInstances();

protected:
	GameStateManager *gsm;
	SystemManager *systemManager;

	bool m_isInitalized = false;

public:
	GameState(GameStateManager *gsm);
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

	virtual void init(entt::registry &engine) = 0;
	virtual void cleanUp(entt::registry &engine) = 0;
	
	virtual void entered(entt::registry &engine) = 0;
	virtual void leaving(entt::registry &engine) = 0;

	virtual void obscuring(entt::registry &engine) = 0;
	virtual void revealing(entt::registry &engine) = 0;

	virtual void update(entt::registry &engine, float delta) = 0;

	bool isInitalized();
};

