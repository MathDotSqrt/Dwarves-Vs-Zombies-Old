#pragma once

namespace Voxel {
	static constexpr int CHUNK_SHIFT_X = 5;
	static constexpr int CHUNK_SHIFT_Y = 7;
	static constexpr int CHUNK_SHIFT_Z = 5;

	static constexpr int CHUNK_WIDTH_X = 1 << CHUNK_SHIFT_X;
	static constexpr int CHUNK_WIDTH_Y = 1 << CHUNK_SHIFT_Y;
	static constexpr int CHUNK_WIDTH_Z = 1 << CHUNK_SHIFT_Z;

	static constexpr int CHUNK_BLOCK_POS_MASK_X = CHUNK_WIDTH_X - 1;
	static constexpr int CHUNK_BLOCK_POS_MASK_Y = CHUNK_WIDTH_Y - 1;
	static constexpr int CHUNK_BLOCK_POS_MASK_Z = CHUNK_WIDTH_Z - 1;

	static constexpr int CHUNK_VOLUME = CHUNK_WIDTH_X * CHUNK_WIDTH_Y * CHUNK_WIDTH_Z;

	static constexpr float CHUNK_RENDER_WIDTH_X = CHUNK_WIDTH_X;
	static constexpr float CHUNK_RENDER_WIDTH_Y = CHUNK_WIDTH_Y;
	static constexpr float CHUNK_RENDER_WIDTH_Z = CHUNK_WIDTH_Z;


}