#include "RenderStateKey.h"

using namespace Graphics;

RenderStateKey::RenderStateKey(
	const ViewPort port, 
	const ViewPortLayer layer, 
	const BlendType type, 
	const MaterialID id, 
	const void *value
) : key(0), value(value){
	
	key |= ((uint32)id << 0);
	key |= ((uint32)type << BLEND_SHIFT);
	key |= ((uint32)layer << LAYER_SHIFT);
	key |= ((uint32)port << PORT_SHIFT);
}

bool RenderStateKey::operator<(const RenderStateKey &other) {
	return key == other.key ? value < other.value : key < other.key;
}

void RenderStateKey::setMaterialID(const MaterialID id) {
	uint64 v = (uint64)id;
	uint64 bits = 1 << MATERIAL_ID_SIZE - 1;
	uint64 mask = ~(bits << 0);
	key = (key & mask) | (v << 0);
}

void RenderStateKey::setBlendType(const BlendType type) {
	uint64 v = (uint64)type;
	uint64 bits = 1 << BLEND_TYPE_SIZE - 1;
	uint64 mask = ~(bits << BLEND_SHIFT);
	key = (key & mask) | (v << BLEND_SHIFT);
}

void RenderStateKey::setViewPortLayer(const ViewPortLayer layer) {
	uint64 v = (uint64)layer;
	uint64 bits = 1 << VIEW_PORT_LAYER_SIZE - 1;
	uint64 mask = ~(bits << LAYER_SHIFT);
	key = (key & mask) | (v << LAYER_SHIFT);
}

void RenderStateKey::setViewPort(const ViewPort port) {
	uint64 v = (uint64)port;
	uint64 bits = 1 << VIEW_PORT_SIZE - 1;
	uint64 mask = ~(bits << PORT_SHIFT);
	key = (key & mask) | (v << PORT_SHIFT);
}

MaterialID RenderStateKey::getMaterialID() const{
	return (MaterialID)(key & MATERIAL_ID_SIZE);
}

BlendType RenderStateKey::getBlendType() const {
	return (BlendType)((key >> BLEND_SHIFT) & BLEND_TYPE_SIZE);
}

ViewPortLayer RenderStateKey::getViewPortLayer() const {
	return (ViewPortLayer)((key >> LAYER_SHIFT) & VIEW_PORT_LAYER_SIZE);
}

ViewPort RenderStateKey::getViewPort() const {
	return (ViewPort)((key >> PORT_SHIFT) & VIEW_PORT_SIZE);
}