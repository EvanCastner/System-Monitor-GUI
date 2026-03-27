#include "network.hpp"
#include <cmath>

// Platform-specific libraries for CPU monitoring
#if defined(_WIN32)
	// Windows API for system time functions
	#include <windows.h>
	#include <iphlpapi.h>

#elif defined(__APPLE__)
	// Sysctl for querying kernel network interface data
	#include <sys/sysctl.h>
	// Network interface structures
	#include <net/if.h>
	// Network interface statistics (if_msghdr, if_data)
	#include <net/if_mib.h>

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
		
		#elif defined(__APPLE__)
			// MacOS implementation using systct1 MIB interface

			// Static variables to track previous byte counts and time
			static uint64_t prevBytesIn  = 0;
			static uint64_t prevBytesOut = 0;
			static bool     initialized  = false;

			// Accumulators for summing across all interfaces
			uint64_t totalBytesIn  = 0;
			uint64_t totalBytesOut = 0;

			// Query the number of network interfaces via systct1
			int mib[] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_SYSTEM, IFMIB_IFCOUNT };
			

	}
}
