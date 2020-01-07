#pragma once
#include "preamble.h"
#include "gl/glew.h"
#include "glm.hpp"
#include <typeinfo>

namespace Graphics {

	namespace Implementation { template<typename> struct Attrib; }

	template<typename unsigned int LOC, typename T>
	class Attrib {
	public:
		typedef enum {
			None = 0,
			Normalize = 1,
		} AttribOption;

		typename typedef T Type;
		typename typedef Implementation::Attrib<T>::ScalarType ScalarType;
		typename typedef Implementation::Attrib<T>::DataType DataType;
		typename typedef Implementation::Attrib<T>::Components Components;

		constexpr Attrib() : 
			dataType(Implementation::Attrib<T>::DefaultDataType),
			components(Implementation::Attrib<T>::DefaultNumComponents),
			attribOption(AttribOption::None) {}

		constexpr Attrib(DataType dataType) :
			dataType(dataType),
			components(Implementation::Attrib<T>::DefaultNumComponents),
			attribOption(AttribOption::None) {}

		constexpr Attrib(DataType dataType, Components components) :
			dataType(dataType),
			components(components),
			attribOption(AttribOption::None) {}

		constexpr Attrib(DataType dataType, Components components, AttribOption attribOption) :
			dataType(dataType),
			components(components),
			attribOption(attribOption){}

		
		constexpr DataType getDataType() const {
			return dataType;
		}

		constexpr int getNumComponents() const {
			return (int)components;
		}

		constexpr AttribOption getAttribOption() const {
			return attribOption;
		}

		constexpr size_t getSizeOfAttrib() const {
			return Implementation::Attrib<T>::size(components, dataType);
		}

	private:
		const DataType dataType;
		const Components components;
		const AttribOption attribOption;
	};

	namespace Implementation {
		template<unsigned int>
		struct AttribSize {
			typedef enum {
				ONE = 1,
				TWO = 2,
				THREE = 3,
				FOUR = 4
			} Components;
		};

		template<>
		struct AttribSize<1> {
			typedef enum {
				ONE = 1,
			} Components;

			constexpr static Components DefaultNumComponents = Components::ONE;
		};

		template<>
		struct AttribSize<2> {
			typedef enum {
				ONE = 1,
				TWO = 2,
			} Components;

			constexpr static Components DefaultNumComponents = Components::TWO;
		};

		template<>
		struct AttribSize<3> {
			typedef enum {
				ONE = 1,
				TWO = 2,
				THREE = 3,
				FOUR = 4,	//todo look at this
			} Components;

			constexpr static Components DefaultNumComponents = Components::THREE;
		};

		template<>
		struct AttribSize<4> {
			typedef enum {
				ONE = 1,
				TWO = 2,
				THREE = 3,
				FOUR = 4,
			} Components;
			
			constexpr static Components DefaultNumComponents = Components::FOUR;
		};

		template<typename T> struct AttribType {};

		template<>
		struct AttribType<double> {
		public:
			typedef double ScalarType;
			typedef enum {
				DOUBLE = GL_DOUBLE
			} DataType;

			constexpr static DataType DefaultDataType = DataType::DOUBLE;

			static size_t size(int components, DataType dataType);
		};

		template<>
		struct AttribType<float> {
		public:
			typedef float ScalarType;
			typedef enum {
				BYTE = GL_BYTE,
				UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
				SHORT = GL_SHORT,
				UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
				INT = GL_INT,
				UNSIGNED_INT = GL_UNSIGNED_INT,
				HALF_FLOAT = GL_HALF_FLOAT,
				FLOAT = GL_FLOAT,

				DOUBLE = GL_DOUBLE
			} DataType;

			constexpr static DataType DefaultDataType = DataType::FLOAT;

			static size_t size(int components, DataType dataType);
		};

		template<>
		struct AttribType<int> {
			typedef int ScalarType;

			typedef enum {
				BYTE = GL_BYTE,
				UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
				SHORT = GL_SHORT,
				UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
				INT = GL_INT,
				UNSIGNED_INT = GL_UNSIGNED_INT,
			} DataType;

			constexpr static DataType DefaultDataType = DataType::INT;
			static size_t size(int components, DataType dataType);
		};

		template<>
		struct AttribType<unsigned int> {
			typedef unsigned int ScalarType;
			typedef AttribType<int>::DataType DataType;
			constexpr static DataType DefaultDataType = DataType::UNSIGNED_INT;

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};

		template<>
		struct AttribType<short> {
			typedef short ScalarType;
			typedef AttribType<int>::DataType DataType;
			constexpr static DataType DefaultDataType = DataType::SHORT;

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};

		template<>
		struct AttribType<unsigned short> {
			typedef unsigned short ScalarType;
			typedef AttribType<int>::DataType DataType;
			
			constexpr static DataType DefaultDataType = DataType::UNSIGNED_SHORT;

			

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};

		template<>
		struct AttribType<signed char> {	//for some reason the compile thinks this is different to char
			typedef char ScalarType;
			typedef AttribType<int>::DataType DataType;
			constexpr static DataType DefaultDataType = DataType::BYTE;

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};

		template<>
		struct AttribType<char> {
			typedef char ScalarType;
			typedef AttribType<int>::DataType DataType;
			constexpr static DataType DefaultDataType = DataType::BYTE;

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};

		template<>
		struct AttribType<unsigned char> {
			typedef unsigned char ScalarType;
			typedef AttribType<int>::DataType DataType;
			constexpr static DataType DefaultDataType = DataType::UNSIGNED_BYTE;

			static size_t size(int components, DataType dataType) {
				return AttribType<int>::size(components, dataType);
			}
		};
		


		//template<typename> struct AttribType {};
		template<typename T> struct Attrib {};

		template<> struct Attrib<double> : AttribType<double>, AttribSize<1> {};
		template<> struct Attrib<float> : AttribType<float>, AttribSize<1> {};
		template<> struct Attrib<int> : AttribType<int>, AttribSize<1> {};
		template<> struct Attrib<unsigned int> : AttribType<unsigned int>, AttribSize<1> {};
		template<> struct Attrib<short> : AttribType<short>, AttribSize<1> {};
		template<> struct Attrib<unsigned short> : AttribType<unsigned short>, AttribSize<1> {};
		template<> struct Attrib<char> : AttribType<char>, AttribSize<1> {};
		template<> struct Attrib<unsigned char> : AttribType<unsigned char>, AttribSize<1> {};


		template<typename T, glm::qualifier Q> struct Attrib<glm::vec<1, T, Q>> : AttribType<T>, AttribSize<1> {};
		template<typename T, glm::qualifier Q> struct Attrib<glm::vec<2, T, Q>> : AttribType<T>, AttribSize<2> {};
		template<typename T, glm::qualifier Q> struct Attrib<glm::vec<3, T, Q>> : AttribType<T>, AttribSize<3> {};
		template<typename T, glm::qualifier Q> struct Attrib<glm::vec<4, T, Q>> : AttribType<T>, AttribSize<4> {};
	}

	typedef Attrib<POSITION_ATTRIB_LOCATION, glm::vec3> PositionAttrib;
	typedef Attrib<NORMAL_ATTRIB_LOCATION, glm::vec3> NormalAttrib;
	typedef Attrib<COLOR_ATTRIB_LOCATION, glm::vec3> ColorAttrib;
	typedef Attrib<TEXCOORD_ATTRIB_LOCATION, glm::vec2> TexcoordAttrib;

}

/*
typedef enum {
				BYTE,
				UNSIGNED_BYTE,
				SHORT,
				UNSIGNED_SHORT,
				INT,
				UNSIGNED_INT,

				//Float - only for float attrib types
				FLOAT,

				//Double - only for float or double attrib types
				Double,

				//Unsigned 10.11.11 packed float - Only for three-component float vector attrib types
				UnsignedInt10f11f11fRev,

				//Unsigned 2.10.10.10 packed integer - Only for four-component float vector attribute type.
				UnsignedInt2101010Rev,

				//Signed 2.10.10.10 packed integer - Only for four-component float vector attribute type
				Int2101010Rev
			} DataType;

*/