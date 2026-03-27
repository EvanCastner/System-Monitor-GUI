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
	// Interface type constants (IFT_LOOP, IFT_ETHER, etc)
	#include <net/if_types.h>

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
			// MacOS implementation using systctl MIB interface

			// Static variables to track previous byte counts and time
			static uint64_t prevBytesIn  = 0;
			static uint64_t prevBytesOut = 0;
			static bool     initialized  = false;

			// Accumulators for summing across all interfaces
			uint64_t totalBytesIn  = 0;
			uint64_t totalBytesOut = 0;

			// Query the number of network interfaces via systct1
			int mib[] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_SYSTEM, IFMIB_IFCOUNT };
			int ifCount = 0;
			size_t ifCountSize = sizeof(ifCount);

			if (sysctl(mib, 5, &ifCount, &ifCountSize, nullptr, 0) == -1){
				return;
			}

			// Iterate over each interace and accumulate byte counts
			for (int i = 1; i <= ifCount; ++i) {
				struct ifmibdata ifmd = {};
				size_t ifmdSize = sizeof(ifmd);

				// Build per-interface MIB: NET -> LINK -> GENERIC -> IFDATA -> index
				int ifMib[] = { CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_IFDATA, i, IFDATA_GENERAL };
				if (sysctl(ifMib, 6, &ifmd, &ifmdSize, nullptr, 0) == -1) {
					continue;
				}

				// Skip lookback interface (lo0) - internal traffic only
				if (ifmd.ifmd_data.ifi_type == IFT_LOOP) {
					continue;
				}

				// Accumulate bytes across all non-loopback interfaces
				totalBytesIn += ifmd.ifmd_data.ifi_ibytes;
				totalBytesOut += ifmd.ifmd_data.ifi_obytes;
			}

			// Initialize baseline values on first run
			if (!initialized) {
				prevBytesIn = totalBytesIn;
				prevBytesOut = totalBytesOut;
				initialized = true;
				net.downloadKBps = 0.0f;
				net.uploadKBps = 0.0f;
			} else {
				// Calculate byte deltas since the last measurment
				uint64_t bytesInDiff = totalBytesIn - prevBytesIn;
				uint64_t bytesOutDiff = totalBytesOut - prevBytesOut;

				// Convert bytes to KB/s
				net.downloadKBps = (float)bytesInDiff = 1240.0f;
				net.uploadKBps   = (float)bytesOutDiff = 1024.0f;
			}

			// Prevent negative values from counter wraps or resets
			if (net.downloadKBps < 0.0f) net.downloadKBps = 0.0f;
			if (net.uploadKBps < 0.0f) net.uploadKBps = 0.0f;

			// Store current byte counts for next iteration
			prevBytesIn  = totalBytesIn;
			prevBytesOut = totalBytesOut;
		
		#elif defined(__linux__)
			// Dummy data for Linux detected network
			net.downloadKBps = 0.0f;
			net.uploadKBps   = 0.0f;
		
		#endif

		// Update running total (convert KB/s to MB, assuming -ls interval)
		net.totalDownloadMB += net.downloadKBps / 1024.0f;
		net.totalUploadMB   += net.uploadKBps   / 1024.0f;

		// Add current speeds to history buffers
		net.downloadHistory.push_back(net.downloadKBps);
		net.uploadHistory.push_back(net.uploadKBps);

		// Maintain rolling window by removing oldest sample if buffer is full
		if (net.downloadHistory.size() > MAX_SAMPLE) {
			net.downloadHistory.erase(net.downloadHistory.begin());
			net.uploadHistory.erase(net.uploadHistory.begin());
		}
	}
}
