#pragma once 

#include <vector>

namespace monitor {
	struct NetworkData {
		float downloadKBps;
		float uploadKBps;
		float totalDownloadMB;
		float totalUploadMB;

		std::vector<float> downloadHistory;
		std::vector<float> uploadHistory;
	};

	void update_network(NetworkData& net);

}
