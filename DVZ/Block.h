#pragma once
#include "common.h"
#include "glm.hpp"
#include <vector>
#include "../DVZ_common/BlockTypeEnum.h"

namespace Voxel {
	//constexpr float BLOCK_RENDER_SIZE = 1.0f;

	enum class MeshType: uint8 {
		MESH_TYPE_NONE,
		MESH_TYPE_BLOCK,
		MESH_TYPE_X
	};

	enum class OccludeType : uint8 {
		OCCLUDE_TYPE_NONE,
		OCCLUDE_TYPE_SELF,
		OCCLUDE_TYPE_ALL
	};

	enum class DrawType : uint8 {
		DRAW_TYPE_OPAQUE,
		DRAW_TYPE_TRANSLUCENT,
		DRAW_TYPE_BLEND,
	};

	enum class LightType : uint8 {
		LIGHT_TYPE_OPAQUE,
		LIGHT_TYPE_TRANSPARENT,
		LIGHT_TYPE_SOURCE
	};

	typedef uint8 UVType;
	typedef glm::vec<4, UVType> UV;

	struct SpriteTexCoords {
		//static constexpr UVType MAX = std::numeric_limits<UVType>::max();
		static constexpr UVType MAX = 1;

		UVType row;
		UVType col;

		SpriteTexCoords() {}
		SpriteTexCoords(UVType row, UVType col);

		/*2---3*/
		/*|   |*/
		/*0---1*/

		UV uv0();
		UV uv1();
		UV uv2();
		UV uv3();
	};

	union BlockTexCoords {
		struct {
			SpriteTexCoords top;
			SpriteTexCoords bottom;
			SpriteTexCoords left;
			SpriteTexCoords right;
			SpriteTexCoords front;
			SpriteTexCoords back;
		};
		SpriteTexCoords texCoords[6];

		BlockTexCoords();
		BlockTexCoords(uint8 r, uint8 c, uint8 numRows, uint8 numCols);
		BlockTexCoords(std::vector<uint8> rc, uint8 numRows, uint8 numCols);
	};

	struct BlockAttribs {
		BlockType blockType;
		MeshType meshType;
		OccludeType occludeType;
		LightType lightType;
		BlockTexCoords texcoords;
		glm::u8vec4 color;

		BlockAttribs();
		BlockAttribs(BlockType, MeshType, OccludeType, BlockTexCoords tex, glm::u8vec4);
		BlockAttribs(BlockType, MeshType, OccludeType, LightType, BlockTexCoords tex, glm::u8vec4);
	};

	class BlockManager {
	private:
		static BlockManager *BLOCK_MANAGER_PTR;
		BlockAttribs blockAttribs[BlockType::BLOCK_TYPE_NUM_TYPES];

	public:
		inline static BlockManager* getInstance() {
			if (BLOCK_MANAGER_PTR == nullptr) {
				BLOCK_MANAGER_PTR = new BlockManager();
			}
			return BLOCK_MANAGER_PTR;
		}

		inline BlockAttribs* getBlockAttrib(BlockType type) {
			BlockAttribs *ptr = blockAttribs;
			return ptr + type;
		}
	private:
		BlockManager();
		void addBlock(BlockAttribs block);
	};


	struct Block {
		BlockType type;
		Block() : type(BlockType::BLOCK_TYPE_DEFAULT){
		}


		Block(BlockType type) : type(type){
		}

		bool operator==(const Block &other) {
			return this->type == other.type;
		}

		bool operator!=(const Block &other) {
			return this->type != other.type;
		}
		
		inline MeshType getMeshType() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->meshType;

		}
		inline OccludeType getOccludeType() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->occludeType;

		}

		inline LightType getLightType() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->lightType;
		}

		inline glm::u8vec4 getColor() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->color;
		}

		inline BlockTexCoords getTexCoords() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->texcoords;
		}

		inline bool isOccludedBy(Block type) {
			OccludeType oType = type.getOccludeType();
			bool isOccluded = false;
			isOccluded |= (oType == OccludeType::OCCLUDE_TYPE_ALL);
			isOccluded |= (oType == OccludeType::OCCLUDE_TYPE_SELF) && (this->type == type.type);

			return isOccluded;
		}
	};

}