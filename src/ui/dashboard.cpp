#include "dashboard.hpp"
#include "monitor/cpu.hpp"
#include "monitor/memory.hpp"
#include "monitor/network.hpp"

#include "imgui.h"
#include "implot.h"

#include <vector>
#include <cmath>


namespace ui {
	void render_dashboard() {
		static monitor::CpuData cpu;
		static monitor::MemoryData mem;
		static monitor::NetworkData net;

		monitor::update_cpu(cpu);
		monitor::update_memory(mem);
		monitor::update_network(net);
		
		ImGui::Begin("System Monitor Dashboard");

		// CPU Usage
		ImGui::Text("CPU Usage Data");
		ImGui::Separator();

		if (ImPlot::BeginPlot("CPU Usage %", ImVec2(-1, 200))) {
			ImPlot::SetupAxes(nullptr, "Usage %", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_None);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

			ImPlot::PlotLine("CPU %", cpu.history.data(), cpu.history.size());

			ImPlot::EndPlot();
		}

		// MEMORY usage
		ImGui::Text("Memory Usage Dummy Data");
		ImGui::Separator();

		ImGui::Text("RAM %lld MB / %lld MB (%.0f%%)",
			mem.usedMB,
			mem.totalMB,
			mem.usagePercent
		);
		// Progress Bar for the Memory data
		ImGui::ProgressBar(mem.usagePercent / 100.0f, ImVec2(-1, 0));

		// NETWORK Usage
		ImGui::Text("Network Usage Dummy Data");
		ImGui::Separator();

		// Plot graph
		if (ImPlot::BeginPlot("Network Speed (KB/s)", ImVec2(-1, 200))) {
			ImPlot::SetupAxes(nullptr, "KB/s", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);

			ImPlot::PlotLine("Download", net.downloadHistory.data(), net.downloadHistory.size());
			ImPlot::PlotLine("Upload",   net.uploadHistory.data(),   net.uploadHistory.size());

			ImPlot::EndPlot();
		}

		ImGui::Text("Download: %.1f KB/s", net.downloadKBps);
		ImGui::Text("Upload:   %.1f KB/s", net.uploadKBps);

		ImGui::Spacing();

		ImGui::Text("Total Downloaded: %.1f MB", net.totalDownloadMB);
		ImGui::Text("Total Uploaded:   %.1f MB", net.totalUploadMB);

		// End the GUI
		ImGui::End();
	}
}
