#pragma once
#include "Common.h"

namespace Util {
	constexpr int32 expand(int x) {
		x &= 0x3FF;
		x = (x | (x << 16)) & 4278190335;
		x = (x | (x << 8)) & 251719695;
		x = (x | (x << 4)) & 3272356035;
		x = (x | (x << 2)) & 1227133513;
		return x;
	}

	constexpr int32 zorder_hash(int i, int j, int k) {
		return expand(i) + (expand(j) << 1) + (expand(k) << 2);
	}
}