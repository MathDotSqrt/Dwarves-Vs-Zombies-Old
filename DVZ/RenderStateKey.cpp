#include "RenderStateKey.h"

using namespace Graphics;

RenderStateKey::RenderStateKey(
	const ViewPort port, 
	const ViewPortLayer layer, 
	const BlendType type, 
	const MaterialID id, 
	const RenderStateKey::ValueType value 
) : key(0), value(value){
	
	key |= (static_cast<KeyType>(id) << 0);
	key |= (static_cast<KeyType>(type) << BLEND_SHIFT);
	key |= (static_cast<KeyType>(layer) << LAYER_SHIFT);
	key |= (static_cast<KeyType>(port) << PORT_SHIFT);
}

bool RenderStateKey::operator<(const RenderStateKey &other) {
	return key == other.key ? value < other.value : key < other.key;
}

void RenderStateKey::setMaterialID(const MaterialID id) {
	KeyType v = static_cast<KeyType>(id);
	KeyType bits = (1 << MATERIAL_ID_SIZE) - 1;
	KeyType mask = ~(bits << 0);
	key = (key & mask) | (v << 0);
}

void RenderStateKey::setBlendType(const BlendType type) {
	KeyType v = static_cast<KeyType>(type);
	KeyType bits = (1 << BLEND_TYPE_SIZE) - 1;
	KeyType mask = ~(bits << BLEND_SHIFT);
	key = (key & mask) | (v << BLEND_SHIFT);
}

void RenderStateKey::setViewPortLayer(const ViewPortLayer layer) {
	KeyType v = static_cast<KeyType>(layer);
	KeyType bits = (1 << VIEW_PORT_LAYER_SIZE) - 1;
	KeyType mask = ~(bits << LAYER_SHIFT);
	key = (key & mask) | (v << LAYER_SHIFT);
}

void RenderStateKey::setViewPort(const ViewPort port) {
	KeyType v = static_cast<KeyType>(port);
	KeyType bits = (1 << VIEW_PORT_SIZE) - 1;
	KeyType mask = ~(bits << PORT_SHIFT);
	key = (key & mask) | (v << PORT_SHIFT);
}

MaterialID RenderStateKey::getMaterialID() const{
	return static_cast<MaterialID>(key & MATERIAL_ID_SIZE);
}

BlendType RenderStateKey::getBlendType() const {
	return static_cast<BlendType>((key >> BLEND_SHIFT) & BLEND_TYPE_SIZE);
}

ViewPortLayer RenderStateKey::getViewPortLayer() const {
	return static_cast<ViewPortLayer>((key >> LAYER_SHIFT) & VIEW_PORT_LAYER_SIZE);
}

ViewPort RenderStateKey::getViewPort() const {
	return static_cast<ViewPort>((key >> PORT_SHIFT) & VIEW_PORT_SIZE);
}

RenderStateKey::ValueType RenderStateKey::getValue() const {
	return value;
}