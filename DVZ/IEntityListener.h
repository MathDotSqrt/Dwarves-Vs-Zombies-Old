#pragma once
#include "entt.hpp"

class IEntityListener
{
public:
	IEntityListener();
	virtual ~IEntityListener();

	//template<typename T>
	//virtual void onConstruct(entt::registry&, entt::entity, T&) = 0;
	//
	//template<typename T>
	//virtual void onReplace(entt::registry&, entt::entity, T&) = 0;

	//template<typename T>
	//virtual void onDestruction(entt::registry&, entt::entity) = 0;
};

