#pragma once 
#include <vector>

namespace moniter {
	struct MemoryData {
		float usagePercent;
		long long totalMB;
		long long usedMB;
	};

	void update_memory(MemoryData& mem);
}
