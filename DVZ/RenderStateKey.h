
#pragma once
#include "common.h"
#include "Material.h"

namespace Graphics {

	//
	enum class ViewPort : uint8 {
		SHADOW = 0,
		FINAL,
		NUM_VIEW_PORTS
	};

	enum class ViewPortLayer : uint8 {
		DEFAULT = 0,
		WORLD,
		SKYBOX,
		NUM_VIEW_PORT_LAYERS
	};
#undef OPAQUE
	enum class BlendType : uint8 {
		OPAQUE = 0,
		ADD,
		SUB,
		MUL,
		NUM_BLEND_TYPES,
	};

	enum class VAOType : uint8 {
		SCENE_INSTANCE = 0,
		CHUNK_RENDER_DATA_COPY,
		NUM_VAO_TYPES
	};

	template<uint32 T>
	constexpr uint32 calc_enum_size() {
		uint32 v = T;
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;

		uint32 r = 0;
		while (v >>= 1) {
			r++;
		}
		return r;
	}

	constexpr uint32 VIEW_PORT_SIZE = calc_enum_size<(uint32)ViewPort::NUM_VIEW_PORTS>();
	constexpr uint32 VIEW_PORT_LAYER_SIZE = calc_enum_size<(uint32)ViewPortLayer::NUM_VIEW_PORT_LAYERS>();
	constexpr uint32 BLEND_TYPE_SIZE = calc_enum_size<(uint32)BlendType::NUM_BLEND_TYPES>();
	constexpr uint32 MATERIAL_ID_SIZE = calc_enum_size<(uint32)MaterialID::NUM_MATERIAL_ID>();
	constexpr uint32 VAO_TYPE_SIZE = calc_enum_size<(uint32)VAOType::NUM_VAO_TYPES>();

	struct RenderStateKey {
		const static uint32 VAO_TYPE_SHIFT = 0;
		const static uint32 MATERIAL_SHIFT = VAO_TYPE_SIZE;
		const static uint32 BLEND_SHIFT = MATERIAL_ID_SIZE + MATERIAL_SHIFT;
		const static uint32 LAYER_SHIFT = BLEND_TYPE_SIZE + BLEND_SHIFT;
		const static uint32 PORT_SHIFT = VIEW_PORT_LAYER_SIZE + LAYER_SHIFT;
		
		typedef uint32 KeyType;
		typedef uint32 ValueType;

		KeyType key; 
		ValueType value;

		
		RenderStateKey(
			const ViewPort port, 
			const ViewPortLayer layer, 
			const BlendType type, 
			const MaterialID id, 
			const ValueType value
		);
		RenderStateKey(
			const ViewPort port, 
			const ViewPortLayer layer, 
			const BlendType type, 
			const MaterialID id, 
			const VAOType vaoType, 
			const ValueType value
		);
		

		bool operator<(const RenderStateKey &other);

		void setVAOType(const VAOType type);
		void setMaterialID(const MaterialID id);
		void setBlendType(const BlendType type);
		void setViewPortLayer(const ViewPortLayer layer);
		void setViewPort(const ViewPort port);

		VAOType getVAOType() const;
		MaterialID getMaterialID() const;
		BlendType getBlendType() const;
		ViewPortLayer getViewPortLayer() const;
		ViewPort getViewPort() const;

		ValueType getValue() const;

	private:
		
	};

	
}



