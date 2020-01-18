#pragma once
#include "entt.hpp"
#include "Components.h"

namespace Event {
	void net_client_connect(entt::registry &registry, entt::entity entity, Component::NetClient &component);
	void net_client_disconnect(entt::registry &registry, entt::entity entity);
}