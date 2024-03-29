#include "Events.h"
#include "SingletonComponents.h"

#include "RakPeerInterface.h"
#include "RakNetTypes.h"

void Event::net_client_connect(entt::registry &registry, entt::entity entity, Component::NetClient &comp) {
	auto &connectedClientMap = registry.ctx<ConnectedClientMap>();
	connectedClientMap[comp.guid] = entity;
}

void Event::net_client_disconnect(entt::registry &registry, entt::entity entity) {
	auto &map = registry.ctx<ConnectedClientMap>();
	auto &component = registry.get<Component::NetClient>(entity);
	map.erase(component.guid);
}