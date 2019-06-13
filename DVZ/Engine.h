#pragma once
#include <entt.hpp>
#include "SystemManager.h"

class Engine : public entt::registry, public SystemManager
{
public:
	Engine();
	~Engine();
};

