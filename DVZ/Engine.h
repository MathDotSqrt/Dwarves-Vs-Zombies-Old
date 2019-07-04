#pragma once
#include "entt.hpp"
#include <list>

class System;

namespace Graphics {
	class OpenGLRenderer;
	class Scene;
}
namespace Network {
	class Client;
}


class Engine : public entt::registry{
private:
	Network::Client *client;
	Graphics::OpenGLRenderer *renderer;
	Graphics::Scene *scene;

	std::list<System*> systems;

	const char* host = "127.0.0.1";
	const unsigned short port = 60000;

public:
	Engine();
	~Engine();

	void connect();
	void update(float delta);
	
	void addSystem(System *system);
	void removeSystem(System *system);
	void updateSystems(float delta);
	void deleteAllActiveSystems();

	Network::Client* getClient();
	Graphics::OpenGLRenderer* getRenderer();
	Graphics::Scene* getScene();
};

