#include "memory.hpp"
#include <string>

namespace monitor {

	static float t = 0.0f;

	void update_memory(MemoryData& mem) {
		// Dummy RAM data
		mem.totalMB = 16000.0f;

		// Dummy usage oscillation
		mem.usagePercent = 50.0f + 30.0f * std::sin(t);
		t += 0.03f;

		mem.usedMB = mem.totalMB * (mem.usagePercent / 100.0f);
	}
}
