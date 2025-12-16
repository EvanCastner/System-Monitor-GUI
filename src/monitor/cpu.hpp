#pragma once 
#include <vector>

namespace monitor {
	struct CpuData {
		float usage;                  // current %
		std::vector<float> history;   // rolling history
	};

	void update_cpu(CpuData& cpu);
}
