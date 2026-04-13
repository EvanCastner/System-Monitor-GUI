#include "dashboard.hpp"
#include "monitor/cpu.hpp"
#include "monitor/memory.hpp"
#include "monitor/network.hpp"

// For GUI rendering
#include "imgui.h"
// For plotting graphs
#include "implot.h"

#include <vector>
#include <cmath>

namespace ui
{
	/**
	 * Renders the main system monitoring dashboard
	 * Creates an ImGui window displaying CPU, memory, and network statistics
	 * with real-time graphs and usage indicators
	 */
	void render_dashboard()
	{
		// Static data structures persist between frames to maintain history
		static monitor::CpuData cpu;
		static monitor::MemoryData mem;
		static monitor::NetworkData net;

		// Update all monitoring data for current frame
		monitor::update_cpu(cpu);
		monitor::update_memory(mem);
		monitor::update_network(net);

		// Create main dashboard window
		ImGui::Begin("System Monitor Dashboard");

		// ==== CPU USAGE SECTION ====
		ImGui::Text("CPU Usage Data");
		ImGui::Separator();

		// Create CPU usage line graph
		if (ImPlot::BeginPlot("CPU Usage %", ImVec2(-1, 200)))
		{
			// Configure axes: no X-axis labels, Y-axis shows percentage
			ImPlot::SetupAxes(nullptr, "Usage %", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_None);
			// Y-axis always ranges from 0-100%
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

			// Get number of historical samples
			int count = cpu.history.size();
			if (count > 0)
			{
				// X-axis spans from 0 to the number of samples
				ImPlot::SetupAxisLimits(ImAxis_X1, 0, count - 1, ImGuiCond_Always);
			}

			// Plot the CPU usage history as a line graph
			ImPlot::PlotLine("CPU %", cpu.history.data(), count);

			// Bug Test
			ImGui::Text("CPU: %.2f%%", cpu.usage);

			ImPlot::EndPlot();
		}

		// ==== MEMORY USAGE SECTION ====
		ImGui::Text("Memory Usage Data");
		ImGui::Separator();

		// Display memory usage in text form: used / total
		ImGui::Text("RAM %lu MB / %lu MB (%.0f%%)",
					mem.usedMB,
					mem.totalMB,
					mem.usagePercent);

		// Visual progress bar showing memory usage
		ImGui::ProgressBar(mem.usagePercent / 100.0f, ImVec2(-1, 0));

		// ==== NETWORK USAGE SECTION ====
		ImGui::Text("Network Usage Data");
		ImGui::Separator();

		// Create network speed graph showing download and upload rates
		if (ImPlot::BeginPlot("Network Speed (KB/s)", ImVec2(-1, 200)))
		{
			// Configure axis: no X-axis labels, Y-axis auto-fits to data range
			ImPlot::SetupAxes(nullptr, "KB/s", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
			// Y-axis always ranges from 0-100%
			ImPlot::SetupAxisLimitsConstraints(ImAxis_Y1, 0.0, IMPLOT_AUTO);
			// Get number of historical samples
			int count = net.downloadHistory.size();
			if (count > 0)
			{
				ImPlot::SetupAxesLimits(ImAxis_X1, 0, count - 1, ImGuiCond_Always);
			}
			// Plot download speed history (shown in one color)
			ImPlot::PlotLine("Download", net.downloadHistory.data(), net.downloadHistory.size());
			// Plot upload speed history (shown in different color)
			ImPlot::PlotLine("Upload", net.uploadHistory.data(), net.uploadHistory.size());

			ImPlot::EndPlot();
		}

		// Display current network speeds
		ImGui::Text("Download: %.1f KB/s", net.downloadKBps);
		ImGui::Text("Upload:   %.1f KB/s", net.uploadKBps);

		ImGui::Spacing();

		// Display cumulative network totals since start
		ImGui::Text("Total Downloaded: %.1f MB", net.totalDownloadMB);
		ImGui::Text("Total Uploaded:   %.1f MB", net.totalUploadMB);

		// Close the dashboard window
		ImGui::End();
	}
}
