#include "dashboard.hpp"
#include "cpu.hpp"
#include "memory.hpp"

#include "imgui.h"
#include "implot.h"

#include <vector>
#include <cmath>


namespace ui {
	void render_dashboard() {
		static monitor::CpuData cpu;
		static monitor::MemoryData mem;

		monitor::update_cpu(cpu);
		monitor::update_memory(mem);
		
		ImGui::Begin("System Moniter Dashboard");

		// CPU Usage
		ImGui::Text("CPU Usage Dummy Data");
		ImGui::Separator();

		if (ImPlot::BeginPlot("CPU Usage %", ImVec2(-1, 200))) {
			ImPlot::SetupAxes(nullptr, "Usage %", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

			ImPlot::PlotLine("CPU %", cpu.history.data(), cpu.history.size());

			ImPlot::EndPlot();
		}

		ImGui::Text("Memory Usage Dummy Data");
		ImGui::Separator();

		ImGui::Text("RAM %.0f MB / %.0f MB ",
			mem.usedMB,
			mem.totalMB,
			mem.usagePercent
		);
		// Progress Bar for the Memory data
		ImGui::ProgressBar(mem.usagePercent / 100.0f, ImVec2(-1, 0));

		ImGui::Text("Network Usage: TODO");

		ImGui::End();
	}
}
