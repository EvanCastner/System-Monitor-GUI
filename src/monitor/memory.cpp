#include "memory.hpp"

#if defined (_WIN32)
	#include <windows.h>

#elif defined (__APPLE__)
	#include <mach/mach.h>

#elif defined (__linux__)
	#include <fstream>
	#include <string>
#endif

namespace monitor {
	void update_memory(MemoryData& mem) {

		#if defined (_WIN32)
			MEMORYSTATUSX statex;
			statex.dwLength = sizeof(statex);
			GlobalMemoryStatusEx(&statex);

			mem.totalMB = statex.ullTotalPhys / (1024 * 1024);
			mem.usedMB = (statex.ullTotalPhys - statex.ullAvailPhys) / (1024 * 1024);
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

		#elif defined (__APPLE__)
			mach_port_t host = mach_host_self();
			vm_statistics64_data_t vm_stat;
			mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;

			host_statistics64(host, HOST_VM_INFO64, (host_info64_t)&vm_stat, &count);
			
			vm_size_t pageSize;
			host_page_size(host, &pageSize);

			uint64_t total = 
				(vm_stat.active_count + 
				 vm_stat.inactive_count +
				 vm_stat.wire_count +
				 vm_stat.free_count) * pageSize;

			uint64_t used = 
				(vm_stat.active_count + 
				 vm_stat.wire_count) * pageSize;

			mem.totalMB = total / (1024 * 1024);
			mem.usedMB = used / (1024 * 1024);
			mem.usagePercent = (float)mem.usedMB * 100.0f / (float)mem.totalMB;

		#endif
	}
}
