#include "PlayState.h"
#include "macrologger.h"


PlayState::PlayState(GameStateManager *gsm) : GameState(gsm) {
}


PlayState::~PlayState() {
}

void PlayState::init() {
	LOG_INFO("init");
}

void PlayState::cleanUp() {
	LOG_INFO("cleanUp");

}

void PlayState::entered() {
	LOG_INFO("entered");

}

void PlayState::leaving() {
	LOG_INFO("leaving");

}

void PlayState::update(float delta) {

}