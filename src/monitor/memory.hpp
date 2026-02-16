#pragma once 
#include <cstdint>    // For uint64_t type

namespace monitor {
	// MemoryData structure: Holds system memory usage information
	struct MemoryData {
		// Current memory usage as a percent
		float usagePercent;
		// Total physical memory in megabytes
		uint64_t totalMB;
		// Currently used memory in megabytes
		uint64_t usedMB;
	};

	/** 
	 * Updates the memory usage statistics 
	 * Queries system-specific APIs to retrieve current memory information
	 * 
	 * @param mem Reference to MemoryData structure to update with the latest measurement
	 * 
	 * Platform suppoer:
	 * - Windows: Uses GlobalMemoryStatusEx() to query physical memory status
	 * - macOS: Uses Mach host_statistics64() to query VM statistics
	 * - Linux: Pending
	 */
	void update_memory(MemoryData& mem);
}
