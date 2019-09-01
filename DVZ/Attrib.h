#include "preamble.glsl"
#include "gl/glew.h"
#include "glm.hpp"
#include <typeinfo>

namespace Graphics {
	template<typename unsigned int LOC, typename T>
	class Attrib {
		typedef enum {
			ONE = 1,
			TWO = 2,
			THREE = 3,
			FOUR = 4
		} Components;

		typedef enum {
			BYTE = GL_BYTE,
			UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
			SHORT = GL_SHORT,
			UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
			INT = GL_INT,
			UNSIGNED_INT = GL_UNSIGNED_INT,
			FLOAT = GL_FLOAT,
			//todo add others later
		} ScalarType;

		typedef enum {
			None = 0,
			Normalize = 1,
		} AttribOption;


		//todo check typeToScalar
		constexpr Attrib(Components components = T::length, ScalarType scalarType = ScalarType::BYTE, AttribOption attribOption = AttribOption::None) : numComponents(components), scalarType(scalarType), attribOption(attribOption){}

		constexpr Components getNumComponents() const {
			return numComponents;
		}

		constexpr ScalarType getScalarType() const {
			return scalarType;
		}

		constexpr AttribOption getAttribOption() const {
			return attribOption;
		}

		constexpr size_t getSizeOfAttrib() const {
			size_t size = 0;

			switch (scalarType) {
			case BYTE:
			case UNSIGNED_BYTE:
				size = sizeof(char);
				break;
			case SHORT:
			case UNSIGNED_SHORT:
				size = sizeof(short);
				break;
			case INT:
			case UNSIGNED_INT:
				size = sizeof(int);
				break;
			case FLOAT:
				size = sizeof(float);
				break;
			}

			return numComponents * size;
		}

	private:
		const Components numComponents;
		const ScalarType scalarType;
		const AttribOption attribOption;

		template<typename T>
		constexpr ScalarType typeToScalarType() const {
			const auto type = typeid(T);


			switch (type) {
			case typeid(char):
			case typeid(glm::int8):
				return ScalarType::BYTE;
			case typeid(unsigned char):
			case typeid(glm::uint8):
				return ScalarType::UNSIGNED_BYTE;
			case typeid(short):
			case typeid(glm::int16):
				return ScalarType::SHORT;
			case typeid(unsigned short):
			case typeid(glm::uint16):
				return ScalarType::UNSIGNED_SHORT;
			case typeid(int):
			case typeid(glm::int32):
				return ScalarType::INT;
			case typeid(unsigned int):
			case typeid(glm::uint32):
				return ScalarType::UNSIGNED_INT;
			case typeid(float):
			case typeid(glm::float32):
				return ScalarType::FLOAT;
			default:
				return ScalarType::BYTE;
			}

			//TODO add checks for this
			return ScalarType::FLOAT;
		}
	};
}