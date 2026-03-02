#include "memory.hpp"

// Platform-specific libraries for memory monitoring
#if defined (_WIN32)
	// Windows API for memory status functions
	#include <windows.h>

#elif defined (__APPLE__)
	// Mach kernal interface for VM statistics
	#include <mach/mach.h>

#elif defined (__linux__)
	// File stream for reading /proc/meminfo
	// String operations for parsing
	#include <fstream>
	#include <string>
#endif

namespace monitor {
	/** 
	 * Updates memory usage statistics for the given MemoryData object
	 * Platform-specific implementation using native APIs 
	 * @param mem Reference to MemoryData structure to update
	*/
	void update_memory(MemoryData& mem) {

		#if defined (_WIN32)
			// Windows implementation using GlobalMemoryStatusEx API

			// Structure to recieve memory status information
			MEMORYSTATUSX statex;
			// Required: set structure size
			statex.dwLength = sizeof(statex);
			// Query system memory information
			GlobalMemoryStatusEx(&statex);  

			// Concery total physical memory from bytes to megabytes
			mem.totalMB = statex.ullTotalPhys / (1024 * 1024);
			// Caculate used memory
			mem.usedMB = (statex.ullTotalPhys - statex.ullAvailPhys) / (1024 * 1024);
			// Calculate usage percentage
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

		#elif defined (__APPLE__)
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
				 vm_stat.free_count) * pageSize;
			
			// Calculate used memory 
			// inactive pages not counted since they can be recalaimed
			uint64_t used = 
				(vm_stat.active_count + 
				 vm_stat.wire_count) * pageSize;
			
			// Convert from bytes to megabytes
			mem.totalMB = total / (1024 * 1024);
			mem.usedMB = used / (1024 * 1024);

			// Calculate useage percentage
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

		#elif defined (__linux__)
			// Dummy data for linux detected memory
			mem.totalMB = 55.0;
			mem.usedMB = 25.0;
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;
		#endif
	}
}
