#pragma once
#include "Common.h"

namespace Math {
	constexpr float PI = 3.14159265359f;
	constexpr float DEGTORAD = PI / 180;
	constexpr float RADTODEG = 180 / PI;

	template<typename T, typename U>
	constexpr T lerp(T a, T b, U t) {
		return a + t * (b - a);
	}
}