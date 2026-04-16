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

		// Overview section
		ImGui::SeparatorText("Overview");

		ImGui::Columns(3, nullptr, false);

		ImGui::Text("CPU");
		ImGui::Text("%.1f%%", cpu.usage);
		ImGui::NextColumn();

		ImGui::Text("Memory");
		ImGui::Text("%.1f%%", mem.usagePercent);
		ImGui::NextColumn();

		ImGui::Text("Network");
		ImGui::Text("%.1f UPLOAD", net.smoothUploadKBps);
		ImGui::Text("%.1f DOWNLOAD", net.smoothDownloadKBps);
		ImGui::Text("Total Downloaded: %.1f MB", net.totalDownloadMB);
		ImGui::Text("Total Uploaded:   %.1f MB", net.totalUploadMB);

		ImGui::Columns(1);
		ImGui::Spacing();

		// ==== CPU USAGE CARD ====
		ImGui::BeginChild("CPUCard", ImVec2(0, 250), true);

		ImGui::Text("CPU");
		ImGui::Separator();

		// Big number
		ImGui::SetWindowFontScale(1.3f);
		ImGui::Text("%.1f%%", cpu.usage);
		ImGui::SetWindowFontScale(1.0f);

		ImGui::Spacing();

		// Create CPU usage line graph
		if (ImPlot::BeginPlot("CPU Usage %", ImVec2(-1, 240)))
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
				// Plot the CPU usage history as a line graph
				ImPlot::PlotLine("CPU %", cpu.history.data(), count);
			}

			ImPlot::EndPlot();
		}

		ImGui::EndChild();

		ImGui::Separator();

		// ==== MEMORY USAGE CARD ====
		ImGui::BeginChild("MemoryCard", ImVec2(0, 100), true);

		ImGui::Text("Memory");
		ImGui::Separator();

		// Big Numb
		ImGui::SetWindowFontScale(1.3f);
		ImGui::Text("%.0f%%", mem.usagePercent);
		ImGui::SetWindowFontScale(1.0f);

		ImGui::Spacing();

		// Visual progress bar showing memory usage
		ImGui::ProgressBar(mem.usagePercent / 100.0f, ImVec2(-1, 12));
		// Display memory usage in text form: used / total
		ImGui::Text("RAM %lu MB / %lu MB", mem.usedMB, mem.totalMB);

		ImGui::EndChild();

		ImGui::Separator();

		// ==== NETWORK USAGE CARD ====
		ImGui::BeginChild("NetworkCard", ImVec2(0, 240), true);

		ImGui::Text("Network");
		ImGui::Separator();

		// Big numbers
		ImGui::SetWindowFontScale(1.3f);
		ImGui::Text("%.1f KB/s", net.smoothDownloadKBps);
		ImGui::SetWindowFontScale(1.0f);

		ImGui::Spacing();

		// Create network speed graph showing download and upload rates
		if (ImPlot::BeginPlot("Network Speed (KB/s)", ImVec2(-1, 200)))
		{
			// Configure axis: no X-axis labels, Y-axis auto-fits to data range
			ImPlot::SetupAxes(nullptr, "KB/s", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
			// Y-axis always ranges
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 0, ImGuiCond_Once);
			// Get number of historical samples
			int count = (int)net.downloadHistory.size();
			if (count > 0)
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, 0, count - 1, ImGuiCond_Always);
				// Plot download speed history (shown in one color)
				ImPlot::PlotLine("Download", net.downloadHistory.data(), net.downloadHistory.size());
				// Plot upload speed history (shown in different color)
				ImPlot::PlotLine("Upload", net.uploadHistory.data(), net.uploadHistory.size());
			}
			ImPlot::EndPlot();
		}

		ImGui::EndChild();

		ImGui::Spacing();

		// Close the dashboard window
		ImGui::End();
	}
}
