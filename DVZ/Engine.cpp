#include "Engine.h"

#include <thread>
#include <chrono>
#include <list>
#include <algorithm>

#include "System.h"
#include "Scene.h"
#include "Client.h"
#include "OpenGLRenderer.h"

#include "Components.h"

using namespace std;

Engine::Engine() {
	this->client = new Network::Client();

	this->scene = new Graphics::Scene();
	this->renderer = new Graphics::OpenGLRenderer();
	this->renderer->init(scene);

}


Engine::~Engine(){
	delete this->client;
	delete this->scene;
	delete this->renderer;

	this->deleteAllActiveSystems();
}

void Engine::connect() {
	this->client->sendConnectionRequest(this->host, this->port);

	while (!this->client->isConnected()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	LOG_ENGINE("CONNECTION ACCEPTED");
}

void Engine::update(float delta) {
	this->client->poll();
	this->updateSystems(delta);

	this->renderer->prerender();
	this->renderer->render();
}

void Engine::addSystem(System *system) {
	if (std::find(this->systems.begin(), this->systems.end(), system) == this->systems.end()) {
		//this->systems.insert(std::lower_bound(this->systems.begin(), this->systems.end(), system), system);
		this->systems.push_back(system);
		system->addedToEngine(this);
	}
}

void Engine::removeSystem(System *system) {
	if (find(this->systems.begin(), this->systems.end(), system) != this->systems.end()) {
		system->removedFromEngine(this);
		this->systems.remove(system);
	}
}

void Engine::updateSystems(float delta) {
	for (auto *system : this->systems) {
		system->update(this, delta);
	}
}

void Engine::deleteAllActiveSystems() {
	for (System *system : this->systems) {
		system->removedFromEngine(this);
		delete system;
	}

	this->systems.clear();
}

Network::Client* Engine::getClient() {
	return this->client;
}

Graphics::OpenGLRenderer* Engine::getRenderer() {
	return this->renderer;
}

Graphics::Scene* Engine::getScene() {
	return this->scene;
}
