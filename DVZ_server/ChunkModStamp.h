#pragma once
#include "Common.h"

namespace Voxel {
	///Defines a chunks modification number
	class ChunkModStamp {
	private:
		int32 mod_counter = 0;

		int32 cx;
		int32 cy;
		int32 cz;
	
	public:

		ChunkModStamp() : cx(-1000), cy(-1000), cz(-1000) {}

		ChunkModStamp(int32 cx, int32 cy, int32 cz) : cx(cx), cy(cy), cz(cz) {
			
		}

		void increment() {
			mod_counter++;
		}

		int32 getModificationCount() {
			return mod_counter;
		}

		void setModificationCount(int32 new_count) {
			assert(new_count > mod_counter);
			mod_counter = new_count;
		}

		int32 getCX() {
			return cx;
		}

		int32 getCY() {
			return cy;
		}

		int32 getCZ() {
			return cz;
		}
	};
}