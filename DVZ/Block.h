#pragma once
#include "common.h"
#include "glm.hpp"
#include <vector>

namespace Voxel {
	//constexpr float BLOCK_RENDER_SIZE = 1.0f;

	typedef enum _BlockType : uint8 {
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
		BLOCK_TYPE_SAND,
		BLOCK_TYPE_GLASS,
		BLOCK_TYPE_GRASS_PLANT,
		BLOCK_TYPE_ROSE,
		BLOCK_TYPE_GOLDENROD,
		BLOCK_TYPE_PURPLE,
		BLOCK_TYPE_INVALID,
		BLOCK_TYPE_NUM_TYPES
	} BlockType;

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
		BlockTexCoords texcoords;
		glm::u8vec4 color;

		BlockAttribs();
		BlockAttribs(BlockType, MeshType, OccludeType, BlockTexCoords tex, glm::u8vec4);
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
		Block() {
			this->type = BlockType::BLOCK_TYPE_DEFAULT;
		}


		Block(BlockType type) {
			this->type = type;
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