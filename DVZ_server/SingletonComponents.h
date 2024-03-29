#pragma once

#include <map>
#include "entt.hpp"
#include "RakNetTypes.h"

struct ConnectedClientMap : std::map<SLNet::RakNetGUID, entt::entity> {
};

struct CloseConnectionBuffer : std::vector<SLNet::RakNetGUID> {

};