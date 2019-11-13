#pragma once

#include "Recycler.h"
#include "Chunk.h"
#include <atomic>

namespace Voxel {
	struct ChunkDestructor {
	public:
		typedef std::atomic<int> RefCount;
		ChunkDestructor(RefCount *ref) {
			this->referenceCount = ref;
			if (referenceCount) {
				(*referenceCount)++;
			}
		}

		void operator()(Chunk *) {
			//decrements reference counter from chunk manager chunkSet
			//when manager sees chunk having 0 references it will eventually
			//recycle that chunk
			if (referenceCount) {
				(*referenceCount)--;
			}
		}
	private:
		RefCount *referenceCount;
	};

	typedef Util::Recycler<Chunk>::UniqueHandle ChunkHandle;
	//typedef std::unique_ptr<Chunk, ChunkDestructor> ChunkRefHandle;

	class ChunkRefHandle : public std::unique_ptr<Chunk, ChunkDestructor> {
	public:
		using std::unique_ptr<Chunk, ChunkDestructor>::unique_ptr;

		ChunkRefHandle() : std::unique_ptr<Chunk, ChunkDestructor>(nullptr, ChunkDestructor(nullptr)) {

		}
	};
}