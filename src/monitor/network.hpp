#pragma once 

#include <vector>

namespace monitor {
	// NetworkData stucture: 
	// Holds network usafe information including current and historical data
	struct NetworkData {
		// Current download speed in kilobytes per second
		float downloadKBps;
		// Current upload speed in kilobytes per second
		float uploadKBps;
		// Cumulative download total in megabytes
		float totalDownloadMB;
		// Cumulative upload total in megabytes
		float totalUploadMB;

		// Rolling history of download speeds
		std::vector<float> downloadHistory;
		// Rolling history of upload speeds 
		std::vector<float> uploadHistory;
	};

	/** 
	 * Updates the network usage statistics
	 * Queries system-specific APIs to calculate current network transfer rates
	 * and maintains rolling history buffers of upload/download speeds
	 * 
	 * @param net Refernce to NetworkData structure to update with latest measurment
	 * 
	 * Platform support:
	 * - Windows: pending
	 * - macOS: pending
	 * - Linux: pending
	 */
	void update_network(NetworkData& net);

}
