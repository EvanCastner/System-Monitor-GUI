#pragma once 
#include <vector>

namespace monitor {
	
	// CpuData structure: holds CPU usage information
	struct CpuData {
		float usage;                  // current CPU usage percentage
		std::vector<float> history;   // rolling history of CPU usage samples
	};

	/** 
	 * Updates the CPU usage statistics
	 * Queries system-specific APIs to calculate current CPU usage and maintains
	 * a rolling history buffer of samples (up to MAX_SAMPLES entries)
	 * 
	 * @param cpu Reference to CpuData structure to update with lateset measurements
	 * 
	 * Platform Support:
	 * - Windows: Uses GetSystemTimes() to measure kernal/user/idle time
	 * - macOS: Uses Mach host_statistics() to query CPU tick counts
	 * - Linux: (Implementation pending)
	 */
	void update_cpu(CpuData& cpu);
}
