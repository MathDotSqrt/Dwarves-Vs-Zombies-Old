#pragma once
#include "entt.hpp"
#include <vector>

namespace SLNet {
	class RakPeerInterface;
}

class EntityAdmin {
public:
	typedef void(*function_ptr)(EntityAdmin &admin);

	
	entt::registry registry;
	EntityAdmin();
	~EntityAdmin();

	void update(float delta);
	void addSystemUpdateFunction(function_ptr func);

	SLNet::RakPeerInterface* getPeer();

private:
	SLNet::RakPeerInterface *peer;
	std::vector<function_ptr> updateFunctions;

};

