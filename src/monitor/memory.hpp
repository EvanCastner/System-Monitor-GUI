#pragma once 
#include <cstdint>

namespace monitor {
	struct MemoryData {
		float usagePercent;
		uint64_t totalMB;
		uint64_t usedMB;
	};

	void update_memory(MemoryData& mem);
}
