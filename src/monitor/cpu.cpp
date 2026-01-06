#include "cpu.hpp"

// Libraries to pull fron any device
#if defined(_WIN32)
 
	#include <windows.h>

#elif defined(_APPLE_)

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
		 	GetSystemTimes(*idleTime, &kernalTime, &userTime);

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

		 #elif defined(_APPLE_)

		 	static uint64_t prevUser = 0, prevSystem = 0, prevIdle = 0, prevNice = 0;
		 	static bool initialized = false;

		 	host_cpu_load_info_data_t info;
		 	mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;

		 	host statistics(mach_host_self(), HOST_CPU_LOAD_INFO, (host_info_t)&info, &count);

		 	uint64_t user = info.cpu_ticks[CPU_STATE_USER];
		 	uint64_t system = info.cpu_ticks[CPU_STATE_SYSTEM];
		 	uint64_t idle = info.cput_ticks[SPU_STATE_IDLE];
		 	uint64_t nice = info.cpu_ticks[CPU_STATE_NICE];

		 	if (!initialized) {
		 		prevUser = user;
		 		prevSystem = system;
		 		prevIdle = idle;
		 		prevNice = nice;
		 		initialized = true;
		 		return;
		 	}

		 	uint64_t total = (user - prevUser) + (system - prevSystem) + (idle - prevIdle) + (nice - prevNice);

		 	if (total > 0) {
		 		cpu.usage = 100.0f * (float)(user - prevUser) + (system - prevSystem) + (nice - prevNice) / (float)total;
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
