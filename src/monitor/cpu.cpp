#include "cpu.hpp"

// Libraries to pull fron any device
#if defined(_WIN32)
 
	#include <windows.h>

#elif defined(__APPLE__)

	#include <mach/mach.h>
	#include <mach/host_info.h>

#elif defined(_linux_)

	#include <fstream>
	#include <sstream>

#endif

namespace monitor {

	static const int MAX_SAMPLES = 200;

	void update_cpu(CpuData& cpu) {

		#if defined(_WIN32)

		 	static ULONGLONG prevIdle = 0, prevKernal = 0, prevUser = 0;

		 	FILETIME idleTime, kernalTime, userTime;
		 	GetSystemTimes(&idleTime, &kernalTime, &userTime);

		 	ULONGLONG idle = *(ULONGLONG*)&idleTime;
		 	ULONGLONG kernal = *(ULONGLONG*)&kernalTime;
		 	ULONGLONG user = *(ULONGLONG*)&userTime;

		 	ULONGLONG idleDiff = idle - prevIdle;
		 	ULONGLONG kernalDiff = kernal - prevKernal;
		 	ULONGLONG userDiff = user - prevUser;

		 	ULONGLONG total = kernalDiff + userDiff;

		 	if (total > 0) {
		 		cpu.usage = 100.0f * (float)(total - idleDiff) / (float)total;
		 	}

		 	prevIdle = idle;
		 	prevKernal = kernal;
		 	prevUser = user;

		 #elif defined(__APPLE__)

		 	static uint64_t prevUser = 0;
		 	static uint64_t prevSystem = 0; 
		 	static uint64_t prevIdle = 0;
		 	static uint64_t prevNice = 0;
		 	static bool initialized = false;

		 	host_cpu_load_info_data_t info;
		 	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

		 	kern_return_t kr = host_statistics(
		 		mach_host_self(),
		 		HOST_CPU_LOAD_INFO,
		 		reinterpret_cast<host_info_t>(&info),
		 		&count
		 	);

		 	if (kr != KERN_SUCCESS) {
		 		return;
		 	}

		 	uint64_t user = info.cpu_ticks[CPU_STATE_USER];
		 	uint64_t system = info.cpu_ticks[CPU_STATE_SYSTEM];
		 	uint64_t idle = info.cpu_ticks[CPU_STATE_IDLE];
		 	uint64_t nice = info.cpu_ticks[CPU_STATE_NICE];

			if (!initialized) {
			    prevUser = user;
			    prevSystem = system;
			    prevIdle = idle;
			    prevNice = nice;
			    initialized = true;
			    cpu.usage = 0.0f;
			} else {
			    uint64_t userDiff   = user   - prevUser;
			    uint64_t systemDiff = system - prevSystem;
			    uint64_t idleDiff   = idle   - prevIdle;
			    uint64_t niceDiff   = nice   - prevNice;

			    uint64_t total = userDiff + systemDiff + idleDiff + niceDiff;

			    if (total > 0) {
			        cpu.usage = 100.0f *
			            (float)(userDiff + systemDiff + niceDiff) /
			            (float)total;
			    }
			}

			prevUser = user;
			prevSystem = system;
			prevIdle = idle;
			prevNice = nice;
			
		 #endif

		 cpu.history.push_back(cpu.usage);
		 if (cpu.history.size() > MAX_SAMPLES) {
		 	cpu.history.erase(cpu.history.begin());
		 }
	}
}
