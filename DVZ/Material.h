#pragma once
namespace Graphics {
	typedef enum _MaterialID : unsigned char {
		NONE_MATERIAL_ID = 0,
		COLOR_MATERIAL_ID,
		NORMAL_MAERIAL_ID,
		BASIC_LIT_MATERIAL_ID,
		TEXTURE_MATERIAL_ID,
		BLOCK_MATERIAL_ID,
		CHUNK_MATERIAL_ID,
		NUM_MATERIAL_ID
	} MaterialID;

	//todo add reference counter for materials
	struct ColorMaterial {
		static MaterialID type;
		float color[3];
	};

	struct NormalMaterial {
		static MaterialID type;
	};

	struct BasicLitMaterial {
		static MaterialID type;

		float diffuseColor[3];
		float specularColor[3];
		float shinyness;
	};

	struct TextureMaterial {
		static MaterialID type;
		unsigned int texID;
	};

	struct BlockMaterial {
		static MaterialID type;
		float specularColor[3];
		float shinyness;
	};
}