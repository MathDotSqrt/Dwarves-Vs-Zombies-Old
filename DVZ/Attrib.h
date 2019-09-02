#pragma once
#include "preamble.glsl"
#include "gl/glew.h"
#include "glm.hpp"
#include <typeinfo>

namespace Graphics {

	namespace Implementation { template<typename> struct Attribute; }

	template<typename unsigned int LOC, typename T>
	class Attrib {
	public:
		typedef enum {
			None = 0,
			Normalize = 1,
		} AttribOption;

		typename typedef Implementation::Attribute<T>::ScalarType ScalarType;
		typename typedef Implementation::Attribute<T>::DataType DataType;
		typename typedef Implementation::Attribute<T>::Components Components;

		//todo check typeToScalar
		constexpr Attrib(
			Components components = Implementation::Attribute<T>::DefaultNumComponents,
			DataType dataType = Implementation::Attribute<T>::DefaultDataType,
			AttribOption attribOption = AttribOption::None
		) : 
			components(components),
			dataType(dataType),
			attribOption(attribOption){}

		constexpr int getNumComponents() const {
			return 3;
		}

		constexpr AttribOption getAttribOption() const {
			return attribOption;
		}

		constexpr size_t getSizeOfAttrib() const {
			return sizeof(T);
		}

	private:
		const Components components;
		const DataType dataType;
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
				ONE = 1
			} Components;

			constexpr static Components DefaultNumComponents = Components::ONE;
		};

		template<>
		struct AttribSize<2> {
			typedef enum {
				ONE = 1,
				TWO = 2
			} Components;

			constexpr static Components DefaultNumComponents = Components::TWO;
		};

		template<>
		struct AttribSize<3> {
			typedef enum {
				ONE = 1,
				TWO = 2,
				THREE = 3
			} Components;

			constexpr static Components DefaultNumComponents = Components::THREE;
		};

		template<>
		struct AttribSize<4> {
			typedef enum {
				ONE = 1,
				TWO = 2,
				THREE = 3,
				FOUR = 4
			} Components;

			constexpr static Components DefaultNumComponents = Components::FOUR;
		};

		template<typename T> struct AttribType {};

		template<>
		struct AttribType<int> {
			typedef int ScalarType;

			typedef enum {
				BYTE,
				UNSIGNED_BYTE,
				SHORT,
				UNSIGNED_SHORT,
				INT,
				UNSIGNED_INT,
			} DataType;

			constexpr static DataType DefaultDataType = DataType::INT;
		};

		template<>
		struct AttribType<float> {
		public:
			typedef float ScalarType;
			typedef enum {
				FLOAT,
				DOUBLE
			} DataType;

			constexpr static DataType DefaultDataType = DataType::FLOAT;
		};


		//template<typename> struct AttribType {};
		template<typename T> struct Attribute {};

		template<> struct Attribute<int> : AttribType<int>, AttribSize<1> {};
		template<> struct Attribute<float> : AttribType<float>, AttribSize<1> {};


		template<typename T, glm::qualifier Q> struct Attribute<glm::vec<1, T, Q>> : AttribType<T>, AttribSize<1> {};
		template<typename T, glm::qualifier Q> struct Attribute<glm::vec<2, T, Q>> : AttribType<T>, AttribSize<2> {};
		template<typename T, glm::qualifier Q> struct Attribute<glm::vec<3, T, Q>> : AttribType<T>, AttribSize<3> {};
		template<typename T, glm::qualifier Q> struct Attribute<glm::vec<4, T, Q>> : AttribType<T>, AttribSize<4> {};
	}

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