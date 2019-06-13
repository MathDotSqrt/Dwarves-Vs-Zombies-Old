#include "MainState.h"
#include "macrologger.h"
#include "PlayState.h"
#include "TestSystem.h"

MainState::MainState(GameStateManager *gsm) : GameState(gsm) {

}


MainState::~MainState() {

}

void MainState::init() {
	LOG_INFO("MainState: init()");

	this->systemManager.addSystem(this->engine, new TestSystem(0));
}

void MainState::cleanUp() {
	LOG_INFO("MainState: cleanUp()");

	this->systemManager.deleteAllActiveSystems(this->engine);

}

void MainState::entered() {
	LOG_INFO("MainState: entered()");

}

void MainState::leaving() {
	LOG_INFO("MainState: leaving()");

}

void MainState::update(float delta) {

	this->systemManager.updateSystems(this->engine, delta);
}