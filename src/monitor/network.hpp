#pragma once 

namespace monitor {
	struct NetworkData {
		float downloaidnKBps;
		float uploadKBps;
		float totalDownloadMB;
		float totalUploadMB;
	};

	void update_network(NetworkData& net);

}
