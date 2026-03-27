#include "network.hpp"
#include <cmath>

// Platform-specific libraries for CPU monitoring
#if defined(_WIN32)
	// Windows API for system time functions
	#include <windows.h>

#elif defined(__APPLE__)
    // Mach kernal and interface
	#include <mach/mach.h>
	// Host CPU Statistics
	#include <mach/host_info.h>

#elif defined(__linux__)
	// File stream operations
	#include <fstream>
	// String stream operations
	#include <sstream>

#endif

namespace monitor {

	// Maximum number of historical network samples that will be retained
	static const int MAX_SAMPLE = 120;

	/** 
	 * Updates network usage statistics for the given NetworkData object
	 * Platform-specific implementation using native APIs
	 * @param net Reference to NetworkData structure to update
	 */
	void update_network(NetworkData& net) {

		#if defined(_WIN32)
			// Dummy data for windows
			net.downloadKBps = 0.0f;
			net.uploadKbps   = 0.0f;

	}
}
