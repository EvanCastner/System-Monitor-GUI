#include "cpu.hpp"
#include <cmath>

namespace monitor {
	static float t = 0.0f;
	static const int MAX_SAMPLES = 200;

	void update_cpu(CpuData& cpu) {
		// Dummy CPU usage
		cpu.usage = 50.0f + 40.0f * std::sin(t);
		t += 0.05f;

		cpu_history.push_back(cpu_usage);
		if (cpu_history.size() > MAX_SAMPLES) {
			cpu_history.erase(cpu_history.begin());
		}
	}
}
