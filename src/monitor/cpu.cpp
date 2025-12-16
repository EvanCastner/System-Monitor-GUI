#include "cpu.hpp"
#include <cmath>

namespace monitor {
	static float t = 0.0f;
	static const int MAX_SAMPLES = 200;

	void update_cpu(CpuData& cpu) {
		// Dummy CPU usage
		cpu.usage = 50.0f + 40.0f * std::sin(t);
		t += 0.05f;

		cpu.history.push_back(cpu.usage);
		if (cpu.history.size() > MAX_SAMPLES) {
			cpu.history.erase(cpu.history.begin());
		}
	}
}
