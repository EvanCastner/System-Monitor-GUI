#include "network.hpp"
#include <cmath>

namespace monitor {

	static float t =0.0f;

	void update_network(NetworkData& net) {
		// Simulated speeds (KB/s)
		net.downloaidnKBps = 500.0f + 400.0f * std::sin(t);
		net.uploadKBps = 120.0f + 100.0f * std::cos(t * 0.08f);

		// Prevent negative numbers
		if (net.downloaidnKBps < 0.0f) net.downloaidnKBps = 0.0f;
		if (net.uploadKBps < 0.0f) net.uploadKBps = 0.0f;

		// Simulated totals
		net.totalDownloadMB += net.downloaidnKBps / 1024.0f / 60.0f;
		net.totalUploadMB += net.uploadKBps / 1024.0f / 60.0f;

		t += 0.05;
	}
}
