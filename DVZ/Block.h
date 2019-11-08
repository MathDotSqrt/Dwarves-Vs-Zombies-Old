#pragma once
#include "common.h"
#include "glm.hpp"

namespace Voxel {
	//constexpr float BLOCK_RENDER_SIZE = 1.0f;

	typedef enum _BlockType : uint8 {
		BLOCK_TYPE_DEFAULT = 0,
		BLOCK_TYPE_STONE,
		BLOCK_TYPE_DIRT,
		BLOCK_TYPE_GRASS,
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

	union BlockTexCoords {
		struct {
			glm::u16vec2 topSpritePos;
			glm::u16vec2 bottomSpritePos;
			glm::u16vec2 leftSpritePos;
			glm::u16vec2 rightSpritePos;
			glm::u16vec2 frontSpritePos;
			glm::u16vec2 backSpritePos;
		};
		glm::u16vec2 texCoords[6];

		BlockTexCoords();
		BlockTexCoords(int r, int c, int numRows, int numCols);
		//BlockTexCoords(glm::i32 coord[6], int numRows, int numCols);
	};

	struct BlockAttribs {
		BlockType blockType;
		MeshType meshType;
		OccludeType occludeType;
		//BlockTexCoords texcoords;
		glm::u8vec4 color;

		BlockAttribs();
		BlockAttribs(BlockType, MeshType, OccludeType, glm::u8vec4);
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
		
		inline MeshType getMeshType() {
			return BlockManager::getInstance()->getBlockAttrib(type)->meshType;

		}
		inline OccludeType getOccludeType() {
			return BlockManager::getInstance()->getBlockAttrib(type)->occludeType;

		}
		inline glm::u8vec4 getColor() const {
			return BlockManager::getInstance()->getBlockAttrib(type)->color;
		}
	};

}