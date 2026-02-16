#include "cpu.hpp"

// Platform-specific libraries for CPU monitoring
#if defined(_WIN32)
	// Windows API for system time functions
	#include <windows.h>

#elif defined(__APPLE__)
    // Mach kernal and interface
	#include <mach/mach.h>
	// Host CPU Statistics
	#include <mach/host_info.h>

#elif defined(_linux_)
	// File stream operations
	#include <fstream>
	// String stream operations
	#include <sstream>

#endif

namespace monitor {
	
	// Maximum number of historical CPU usage samples to retain
	static const int MAX_SAMPLES = 200;


	/** 
	 * Updates CPU usage statistics for the given CpuData object
	 * Platform-specifc implementation using native APIs
	 * @param cpu Reference to Cpu Data structure to update
	 */
	void update_cpu(CpuData& cpu) {

		#if defined(_WIN32)
			// Windows implementation using GetSystems API

			// Static variables to store previous tick counts
		 	static ULONGLONG prevIdle = 0, prevKernal = 0, prevUser = 0;

			// Retrive current system times (100-nanosecond intervals)
		 	FILETIME idleTime, kernalTime, userTime;
		 	GetSystemTimes(&idleTime, &kernalTime, &userTime);

			// Convert FILETIME structures to 64-bit integers
		 	ULONGLONG idle = *(ULONGLONG*)&idleTime;
		 	ULONGLONG kernal = *(ULONGLONG*)&kernalTime;
		 	ULONGLONG user = *(ULONGLONG*)&userTime;

			// Calculate time deltas since the last mesaurement
		 	ULONGLONG idleDiff = idle - prevIdle;
		 	ULONGLONG kernalDiff = kernal - prevKernal;
		 	ULONGLONG userDiff = user - prevUser;

			// Total CPU time
		 	ULONGLONG total = kernalDiff + userDiff;

			// Calculate CPU usage percentage
		 	if (total > 0) {
		 		cpu.usage = 100.0f * (float)(total - idleDiff) / (float)total;
		 	}

			// Store current values for next iteration
		 	prevIdle = idle;
		 	prevKernal = kernal;
		 	prevUser = user;

		 #elif defined(__APPLE__)
			// MacOS implementation using Mach Kernal APIs

			// Static varaibles to track previous CPU tick counts
		 	static uint64_t prevUser = 0;
		 	static uint64_t prevSystem = 0; 
		 	static uint64_t prevIdle = 0;
		 	static uint64_t prevNice = 0;
		 	static bool initialized = false;    // First run flag

			// Query host CPU statistics
		 	host_cpu_load_info_data_t info;
		 	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

		 	kern_return_t kr = host_statistics(
		 		mach_host_self(),
		 		HOST_CPU_LOAD_INFO,
		 		reinterpret_cast<host_info_t>(&info),
		 		&count
		 	);

			// Early return if kernal call failed
		 	if (kr != KERN_SUCCESS) {
		 		return;
		 	}

			// Extract CPU ticks for different states
		 	uint64_t user = info.cpu_ticks[CPU_STATE_USER];       // User mode
		 	uint64_t system = info.cpu_ticks[CPU_STATE_SYSTEM];   // Kernal mode
		 	uint64_t idle = info.cpu_ticks[CPU_STATE_IDLE];       // Idle 
		 	uint64_t nice = info.cpu_ticks[CPU_STATE_NICE];       // Low priority user mode

			// Initialize baseline values on first one
			if (!initialized) {
			    prevUser = user;
			    prevSystem = system;
			    prevIdle = idle;
			    prevNice = nice;
			    initialized = true;
			    cpu.usage = 0.0f;
			} else {
				// Calculate tick deltas since last measurement
			    uint64_t userDiff   = user   - prevUser;
			    uint64_t systemDiff = system - prevSystem;
			    uint64_t idleDiff   = idle   - prevIdle;
			    uint64_t niceDiff   = nice   - prevNice;

				// Total ticks across all CPU states
			    uint64_t total = userDiff + systemDiff + idleDiff + niceDiff;

				// Calculate usage
			    if (total > 0) {
			        cpu.usage = 100.0f *
			            (float)(userDiff + systemDiff + niceDiff) /
			            (float)total;
			    }
			}

			// Update previous values for next iteration
			prevUser = user;
			prevSystem = system;
			prevIdle = idle;
			prevNice = nice;
			
		 #endif

		 // Linux implementation is pending
		
		// Add current usage to history buffer
		cpu.history.push_back(cpu.usage);

		// Maintain rolling window by removing oldest sample if buffer is full
		if (cpu.history.size() > MAX_SAMPLES) {
			cpu.history.erase(cpu.history.begin());
		}
	}
}
