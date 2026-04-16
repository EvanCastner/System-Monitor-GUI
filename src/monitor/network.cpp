#include "network.hpp"
#include <cmath>
#include <cstdint>
#include <chrono>
// Error messages and debug messages
#include <iostream>

// Platform-specific libraries for CPU monitoring
#if defined(_WIN32)
// Must be included before windows.h to prevent winsock.h conflict
#include <WinSock2.h>
// Windows API for system time and memory functions
#include <windows.h>
// Network interface table API (MIB_IF_TABLE2, GetIfTable2)
#include <netioapi.h>
// IP helper functions (GetIfTable2, FreeMibTable)
#include <iphlpapi.h>
// Process and memory status API (MEMORYSTATUSEX)
#include <psapi.h>

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
#include <iostream>

#endif

namespace monitor
{

	// Maximum number of historical network samples that will be retained
	static const int MAX_SAMPLE = 120;

	/**
	 * Updates network usage statistics for the given NetworkData object
	 * Platform-specific implementation using native APIs
	 * @param net Reference to NetworkData structure to update
	 */
	void update_network(NetworkData &net)
	{

#if defined(_WIN32)
		// Windows implementation using GetIfTable2 API

		// Static variables to track previous byte count and time
		static uint64_t prevBytesIn = 0;
		static uint64_t prevBytesOut = 0;
		static bool initialized = false;
		static auto prevTime = std::chrono::steady_clock::now();

		// Request the full interface table from Windows
		MIB_IF_TABLE2 *ifTable = nullptr;
		if (GetIfTable2(&ifTable) == NO_ERROR)
		{
			// Accumulators for summing across all interfaces
			uint64_t totalBytesIn = 0;
			uint64_t totalBytesOut = 0;

			// Iterate over each interface and accumulate byte counts
			for (ULONG i = 0; i < ifTable->NumEntries; i++)
			{
				MIB_IF_ROW2 &row = ifTable->Table[i];

				// Skip loopback and tunnel interfaces
				if (row.Type == IF_TYPE_SOFTWARE_LOOPBACK)
					continue;
				if (row.Type == IF_TYPE_TUNNEL)
					continue;
				// Only count interfaces that are up and connected
				if (row.OperStatus != IfOperStatusUp)
					continue;

				// Acculumate bytes across al active non-loopback interfaces
				totalBytesIn += row.InOctets;
				totalBytesOut += row.OutOctets;
			}

			// Free the interface table to avoid memory leaks
			FreeMibTable(ifTable);

			// Calculate elapsed time since last measurment
			auto currentTime = std::chrono::steady_clock::now();
			float deltaSeconds = std::chrono::duration<float>(currentTime - prevTime).count();

			// Initialize baseline values on first run
			if (!initialized)
			{
				prevBytesIn = totalBytesIn;
				prevBytesOut = totalBytesOut;
				prevTime = currentTime;
				initialized = true;
				net.downloadKBps = 0.0f;
				net.uploadKBps = 0.0f;
			}
			else
			{
				// Calculate byte deltas since the last measurement
				uint64_t bytesInDiff = totalBytesIn - prevBytesIn;
				uint64_t bytesOutDiff = totalBytesOut - prevBytesOut;

				// Convert bytes to KB/s using elapsed time
				if (deltaSeconds > 0.0f)
				{
					net.downloadKBps = (bytesInDiff / 1024.0f) / deltaSeconds;
					net.uploadKBps = (bytesOutDiff / 1024.0f) / deltaSeconds;
				}
				else
				{
					net.downloadKBps = 0.0f;
					net.uploadKBps = 0.0f;
				}

				// Store current values for next iteration
				prevBytesIn = totalBytesIn;
				prevBytesOut = totalBytesOut;
				prevTime = currentTime;
			}
		}

#elif defined(__APPLE__)
		// MacOS implementation using systctl MIB interface

		// Static variables to track previous byte counts and time
		static uint64_t prevBytesIn = 0;
		static uint64_t prevBytesOut = 0;
		static bool initialized = false;
		static auto prevTime = std::chrono::steady_clock::now();

		// Only sample every 0.5 seconds
		static auto lastSampleTime = std::chrono::steady_clock::now();
		auto now = std::chrono::steady_clock::now();

		// Accumulators for summing across all interfaces
		uint64_t totalBytesIn = 0;
		uint64_t totalBytesOut = 0;

		// Query the number of network interfaces via systct1
		int mib[] = {CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_SYSTEM, IFMIB_IFCOUNT};
		int ifCount = 0;
		size_t ifCountSize = sizeof(ifCount);
		if (sysctl(mib, 5, &ifCount, &ifCountSize, nullptr, 0) == -1)
		{
			return;
		}

		// Iterate over each interace and accumulate byte counts
		for (int i = 1; i <= ifCount; ++i)
		{
			struct ifmibdata ifmd = {};
			size_t ifmdSize = sizeof(ifmd);

			// Build per-interface MIB: NET -> LINK -> GENERIC -> IFDATA -> index
			int ifMib[] = {CTL_NET, PF_LINK, NETLINK_GENERIC, IFMIB_IFDATA, i, IFDATA_GENERAL};
			if (sysctl(ifMib, 6, &ifmd, &ifmdSize, nullptr, 0) == -1)
			{
				continue;
			}

			if (ifmd.ifmd_name[0] == 'l' && ifmd.ifmd_name[1] == 'o')
			{
				continue;
			}

			// Accumulate bytes across all non-loopback interfaces
			totalBytesIn += ifmd.ifmd_data.ifi_ibytes;
			totalBytesOut += ifmd.ifmd_data.ifi_obytes;
		}

		// Initialize baseline values on first run
		if (!initialized)
		{
			prevBytesIn = totalBytesIn;
			prevBytesOut = totalBytesOut;
			prevTime = now;
			initialized = true;
			net.downloadKBps = 0.0f;
			net.uploadKBps = 0.0f;
			net.downloadHistory.clear();
			net.uploadHistory.clear();
		}

		float timeSinceLastSample = std::chrono::duration<float>(now - lastSampleTime).count();

		if (timeSinceLastSample >= 0.5f)
		{
			float deltaSeconds = std::chrono::duration<float>(now - prevTime).count();

			uint64_t bytesInDiff = totalBytesIn - prevBytesIn;
			uint64_t bytesOutDiff = totalBytesOut - prevBytesOut;

			if (deltaSeconds > 0.0f)
			{
				net.downloadKBps = (bytesInDiff / 1024.0f) / deltaSeconds;
				net.uploadKBps = (bytesOutDiff / 1024.0f) / deltaSeconds;
			}

			prevBytesIn = totalBytesIn;
			prevBytesOut = totalBytesOut;
			prevTime = now;
			lastSampleTime = now;
		}

		const float alpha = 0.3f;
		// Initialize on first valid sample
		if (net.smoothDownloadKBps == 0.0f)
		{
			net.smoothDownloadKBps = net.downloadKBps;
			net.smoothUploadKBps = net.uploadKBps;
		}
		else
		{
			net.smoothDownloadKBps = alpha * net.downloadKBps + (1.0f - alpha) * net.smoothDownloadKBps;
			net.smoothUploadKBps = alpha * net.uploadKBps + (1.0f - alpha) * net.smoothUploadKBps;
		}

#elif defined(__linux__)
		// Linux implementation using /proc/net/dev

		// Open file
		std::ifstream netFile("/proc/net/dev");
		if (!netFile.is_open())
		{
			net.downloadKBps = 0.0f;
			net.uploadKBps = 0.0f;
			std::cerr << "Network data could not be retrieved" << std::endl;
		}
		else
		{
			std::string line;
			uint64_t totalBytesIn = 0;
			uint64_t totalBytesOut = 0;

			// Skip the first two header lines
			std::getline(netFile, line);
			std::getline(netFile, line);

			// Read each interface line
			while (std::getline(netFile, line))
			{
				std::istringstream iss(line);
				std::string iface;

				uint64_t bytesIn = 0, bytesOut = 0;

				// Read the interface
				iss >> iface;

				// Remove the colon from interface name
				if (iface.back() == ':')
					iface.pop_back();

				// Skip the loopback
				if (iface == "lo")
					continue;

				// Read bytes
				iss >> bytesIn;

				// Skip next 7 fields
				uint64_t field;
				for (int i = 0; i < 7; ++i)
					iss >> field;

				// Read bytes
				iss >> bytesOut;

				totalBytesIn += bytesIn;
				totalBytesOut += bytesOut;
			}

			// Static variables to track previous byte counts
			static uint64_t prevBytesIn = 0;
			static uint64_t prevBytesOut = 0;
			static bool initialized = false;
			static auto prevTime = std::chrono::steady_clock::now();

			auto currentTime = std::chrono::steady_clock::now();
			float deltaSeconds = std::chrono::duration<float>(currentTime - prevTime).count();

			// initialize baseline on first run
			if (!initialized)
			{
				prevBytesIn = totalBytesIn;
				prevBytesOut = totalBytesOut;
				prevTime = currentTime;
				initialized = true;
				net.downloadKBps = 0.0f;
				net.uploadKBps = 0.0f;
			}
			else
			{
				// Compute bytes difference since the last measurment
				uint64_t bytesInDiff = totalBytesIn - prevBytesIn;
				uint64_t bytesOutDiff = totalBytesOut - prevBytesOut;

				// Convert to seconds and updates on delta time
				if (deltaSeconds > 0.0f)
				{
					net.downloadKBps = (bytesInDiff / 1024.0f) / deltaSeconds;
					net.uploadKBps = (bytesOutDiff / 1024.0f) / deltaSeconds;
				}
				else
				{
					net.downloadKBps = 0.0f;
					net.uploadKBps = 0.0f;
				}

				prevTime = currentTime;
			}

			// Update previous values for next iteration
			prevBytesIn = totalBytesIn;
			prevBytesOut = totalBytesOut;
		}

#endif

		// Update running total (convert KB/s to MB, assuming -ls interval)
		net.totalDownloadMB += net.downloadKBps / 1024.0f;
		net.totalUploadMB += net.uploadKBps / 1024.0f;

		// Add current speeds to history buffers
		net.downloadHistory.push_back(net.smoothDownloadKBps);
		net.uploadHistory.push_back(net.smoothUploadKBps);

		// Maintain rolling window by removing oldest sample if buffer is full
		if (net.downloadHistory.size() > MAX_SAMPLE)
		{
			net.downloadHistory.erase(net.downloadHistory.begin());
			net.uploadHistory.erase(net.uploadHistory.begin());
		}
	}
}
