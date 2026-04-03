#include "memory.hpp"

// Platform-specific libraries for memory monitoring
#if defined(_WIN32)
// Windows API for memory status functions
#include <windows.h>

#elif defined(__APPLE__)
// Mach kernal interface for VM statistics
#include <mach/mach.h>

#elif defined(__linux__)
// File stream for reading /proc/meminfo
// String operations for parsing
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#endif

namespace monitor
{
	/**
	 * Updates memory usage statistics for the given MemoryData object
	 * Platform-specific implementation using native APIs
	 * @param mem Reference to MemoryData structure to update
	 */
	void update_memory(MemoryData &mem)
	{

#if defined(_WIN32)
// Header file only found in windows code
#include <windows.h>
		// Windows implementation using GlobalMemoryStatusEx API

		// Structure to receive memory status information
		MEMORYSTATUSEX statex;
		// Required: set structure size
		statex.dwLength = sizeof(statex);
		// Query system memory information
		GlobalMemoryStatusEx(&statex);

		// Convert total physical memory from bytes to megabytes
		mem.totalMB = statex.ullTotalPhys / (1024 * 1024);
		// Calculate used memory
		mem.usedMB = (statex.ullTotalPhys - statex.ullAvailPhys) / (1024 * 1024);
		// Calculate usage percentage
		mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

#elif defined(__APPLE__)
		// macOS implementation using Mach VM statistics

		// Get handle to the host
		mach_port_t host = mach_host_self();
		// Structure to recieve virtual memory statistics
		vm_statistics64_data_t vm_stat;
		mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;

		// Query VM statistics from kernal
		host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count);

		// Ger the system page size
		vm_size_t pageSize;
		host_page_size(host, &pageSize);

		// Calculate total memory
		// active: Pages currently in use
		// inactive: Pages not recently use but still in RAM
		// wire: Pages licked un memory (kernal use)
		// free: Unused pages
		uint64_t total =
			(vm_stat.active_count +
			 vm_stat.inactive_count +
			 vm_stat.wire_count +
			 vm_stat.free_count) *
			pageSize;

		// Calculate used memory
		// inactive pages not counted since they can be recalaimed
		uint64_t used =
			(vm_stat.active_count +
			 vm_stat.wire_count) *
			pageSize;

		// Convert from bytes to megabytes
		mem.totalMB = total / (1024 * 1024);
		mem.usedMB = used / (1024 * 1024);

		// Calculate useage percentage
		mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

#elif defined(__linux__)
		// Linux implementation using /proc/meminfo

		// Parse memory info
		// Open the file
		std::ifstream memFile("/proc/meminfo");
		if (!memFile.is_open())
		{
			mem.totalMB = 0;
			mem.usedMB = 0;
			mem.usagePercent = 0.0f;

			// Print a warning to console
			std::cerr << "[Memory Monitor] ERROR: Could not open /proc/meminfo" << std::endl;
		}
		else
		{
			std::string line;
			uint64_t memTotalKB = 0;
			uint64_t memAvailableKB = 0;

			// Read the file line by line
			while (std::getline(memFile, line))
			{
				std::istringstream iss(line);
				std::string key;
				uint64_t value;
				std::string unit;

				iss >> key >> value >> unit;

				if (key == "MemTotal:")
				{
					// Total physical memory in system
					memTotalKB = value;
				}
				else if (key == "MemAvailable:")
				{
					// Memory available for apps in KB
					memAvailableKB = value;
				}

				// Stop reading if both values are found
				if (memTotalKB && memAvailableKB)
				{
					break;
				}
			}

			// Convert KB to MB
			mem.totalMB = memTotalKB / 1024;
			mem.usedMB = (memTotalKB - memAvailableKB) / 1024;

			// Calculate the usage percentage
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;
		}
#endif
	}
}
