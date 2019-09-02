#include "Attrib.h"

using namespace Graphics::Implementation;

size_t AttribType<int>::size(int components, AttribType::DataType dataType) {
	switch (dataType) {
	case DataType::UNSIGNED_BYTE:
	case DataType::BYTE:
		return components;
	case DataType::UNSIGNED_SHORT:
	case DataType::SHORT:
		return 2 * components;
	case DataType::UNSIGNED_INT:
	case DataType::INT:
		return 4 * components;
	}

	assert(0);
	return 0;
}

size_t AttribType<float>::size(int components, AttribType::DataType dataType) {
	switch (dataType) {
	case DataType::UNSIGNED_BYTE:
	case DataType::BYTE:
		return components;
	case DataType::UNSIGNED_SHORT:
	case DataType::SHORT:
	case DataType::HALF_FLOAT:
		return 2 * components;
	case DataType::UNSIGNED_INT:
	case DataType::INT:
	case DataType::FLOAT:
		return 4 * components;
	case DataType::DOUBLE:
		return 8 * components;
	}

	assert(0);
	return 0;
}

size_t AttribType<double>::size(int components, AttribType::DataType dataType) {
	switch (dataType) {
	case DataType::DOUBLE:
		return 8 * components;
	}

	assert(0);
	return 0;
}