#pragma once
#include <glm.hpp>
#include "Geometry.h"
#include "PackedFreeList.h"
#include "VAO.h"
#include "VBO.h"
namespace Graphics  {

/*
0000 0000   0000 0000   0000 0000   0000 0000

layer, transparancy type, cmd, material id, texture id
000 00 0 00000 00000
*/


typedef enum _MaterialID: unsigned char {
	NONE_MATERIAL_ID = 0,
	COLOR_MATERIAL_ID,
	NORMAL_MAERIAL_ID,
	BASIC_LIT_MATERIAL_ID,
	TEXTURE_MATERIAL_ID,
	BLOCK_MATERIAL_ID
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
	GLuint texID;
};

struct BlockMaterial {
	static MaterialID type;
	float specularColor[3];
	float shinyness;
};

struct VertexBuffer {
	VBO vbo;
	unsigned int reference_count;

	VertexBuffer() : vbo(GL_ARRAY_BUFFER){
		this->reference_count = 0;
	}
};

//todo add reference count to mesh to delete it when no transformations are applied to it
struct Mesh {
	VAO vao;
	VBO ebo;
	int indexCount;
	
	unsigned int vboID;
	
	MaterialID typeID;
	unsigned int materialInstanceID;

	Mesh() : ebo(GL_ELEMENT_ARRAY_BUFFER){
		this->indexCount = 0;
		this->vboID = 0;
		this->typeID = MaterialID::NONE_MATERIAL_ID;
		this->materialInstanceID = 0;
	}

	//Mesh(MaterialID typeID, unsigned int materialInstanceID) : vbo(GL_ARRAY_BUFFER), ebo(GL_ELEMENT_ARRAY_BUFFER) {
	//	this->indexCount = 0;
	//	this->typeID = typeID;
	//	this->materialInstanceID = materialInstanceID;
	//}
};

struct Transformation {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

struct Instance {
	unsigned int meshID;
	unsigned int transformationID;
};

struct PointLight {
	glm::vec3 position;
	float color[3];
	float intensity;
};

//annoying bug where RakPeerInterface is redefining these vaiable names
#undef near
#undef far

struct Camera {
	glm::vec3 eye;
	glm::vec3 target;
	glm::vec3 up;

	float fov;
	float aspect;
	float near;
	float far;
};




class Scene {
private:
	unsigned int mainCameraID;

public:
	Util::PackedFreeList<Camera> cameraCache;
	Util::PackedFreeList<Instance> instanceCache;
	Util::PackedFreeList<Transformation> transformationCache;
	Util::PackedFreeList<Mesh> meshCache;
	Util::PackedFreeList<VertexBuffer> vertexBufferCache;

	Util::PackedFreeList<ColorMaterial> colorMaterialCache;
	Util::PackedFreeList<BasicLitMaterial> basicLitMaterialCache;
	Util::PackedFreeList<TextureMaterial> textureMaterialCache;
	Util::PackedFreeList<BlockMaterial> blockMaterialCache;

	Util::PackedFreeList<PointLight> pointLightCache;

	Scene();
	~Scene();

	template<typename VERTEX, typename ...T>
	unsigned int createVertexBuffer(const Geometry<VERTEX, T...> &geometry) {
		
	}

	template<typename MATERIAL, typename VERTEX, typename ...T>
	unsigned int createMesh(const Geometry<VERTEX, T...> &model, MATERIAL &material) {
		unsigned int materialInstanceID = this->createMaterialInstance(material);
		//unsigned int bufferInstanceID = this->create
		
		Mesh newMesh;
		
		
		unsigned int newMeshID = this->meshCache.insert(std::move(newMesh));
		return 0;
	}


	template<typename MATERIAL>
	unsigned int createMaterialInstance(MATERIAL &material) {
		return 0;
	}

	unsigned int createMaterialInstance(ColorMaterial &material);
	unsigned int createMaterialInstance(BasicLitMaterial &material);
	unsigned int createMaterialInstance(TextureMaterial &material);
	unsigned int createMaterialInstance(BlockMaterial &material);

	unsigned int createRenderInstance(unsigned int meshID, Transformation t);
	unsigned int createRenderInstance(unsigned int meshID, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
	unsigned int createRenderInstance(unsigned int meshID);
	
	unsigned int createCameraInstance(Camera camera);

	unsigned int createPointLightInstance(PointLight &p);
	unsigned int createPointLightInstance();


	void removeMaterialInstance(MaterialID materialType, unsigned int materialID);
	void removeMesh(unsigned int meshID);
	void removeTransformation(unsigned int transformationID);
	void removeRenderInstance(unsigned int instanceID);

	void setMainCamera(unsigned int cameraID);
	unsigned int getMainCameraID();
};
}
