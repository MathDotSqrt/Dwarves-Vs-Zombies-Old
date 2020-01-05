#include "MainState.h"
#include "macrologger.h"
#include "PlayState.h"
#include "Components.h"

#include "VAO.h"
#include "VBO.h"
#include "ShaderSet.h"

MainState::MainState(GameStateManager *gsm) : GameState(gsm) {

}


MainState::~MainState() {

}

void MainState::init() {
	LOG_INFO("MainState: init()");
}

void MainState::cleanUp() {
	LOG_INFO("MainState: cleanUp()");
}

void MainState::entered() {

}

void MainState::leaving() {
	LOG_INFO("MainState: leaving()");

}

void MainState::update(float delta) {
}