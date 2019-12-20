#include "RenderStateKey.h"

using namespace Graphics;

RenderStateKey::RenderStateKey(
	const ViewPort port, 
	const ViewPortLayer layer, 
	const BlendType type, 
	const MaterialID id, 
	const RenderStateKey::ValueType value 
) : key(0), value(value){
	setVAOType(VAOType::SCENE_INSTANCE);
	setMaterialID(id);
	setBlendType(type);
	setViewPortLayer(layer);
	setViewPort(port);
}

RenderStateKey::RenderStateKey(
	const ViewPort port,
	const ViewPortLayer layer,
	const BlendType type,
	const MaterialID id,
	const VAOType vaoType,
	const RenderStateKey::ValueType value
) : key(0), value(value) {
	setVAOType(vaoType);
	setMaterialID(id);
	setBlendType(type);
	setViewPortLayer(layer);
	setViewPort(port);
}

bool RenderStateKey::operator<(const RenderStateKey &other) {
	return key == other.key ? value < other.value : key < other.key;
}

void RenderStateKey::setVAOType(const VAOType type) {
	KeyType v = static_cast<KeyType>(type);
	KeyType bits = (1 << VAO_TYPE_SIZE) - 1;
	KeyType mask = ~(bits << VAO_TYPE_SHIFT);
	key = (key & mask) | (v << VAO_TYPE_SHIFT);
}

void RenderStateKey::setMaterialID(const MaterialID id) {
	KeyType v = static_cast<KeyType>(id);
	KeyType bits = (1 << MATERIAL_ID_SIZE) - 1;
	KeyType mask = ~(bits << MATERIAL_SHIFT);
	key = (key & mask) | (v << MATERIAL_SHIFT);
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

VAOType RenderStateKey::getVAOType() const {
	return static_cast<VAOType>((key >> VAO_TYPE_SHIFT) & VAO_TYPE_SIZE);
}

MaterialID RenderStateKey::getMaterialID() const{
	return static_cast<MaterialID>((key >> MATERIAL_SHIFT) & MATERIAL_ID_SIZE);
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