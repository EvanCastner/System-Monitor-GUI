#include "dashboard.hpp"

#include "imgui.h"
#include "implot.h"

#include <vector>
#include <cmath>


namespace ui {
	void render_dashboard() {
		static moniter::CpuData cpu;

		moniter::update_cpu(cpu);
		
		ImGui::Begin("System Moniter Dashboard");

		// CPU Usage
		ImGui::Text("CPU Usage Dummy Data");
		ImGui::Separator();

		if (ImPlot::BeginPlot("CPU Usage %", ImVec2(-1, 200))) {
			ImPlot::SetupAxes(nullptr, "Usage %", ImPlotAxisFlags_NoTickLabels, ImPlotAxisFlags_AutoFit);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100, ImGuiCond_Always);

			ImPlot::PlotLine("CPU %", cpu_history.data(), cpu_history.size());

			ImPlot::EndPlot();
		}

		ImGui::Text("Memory Usage: TODO");
		ImGui::Text("Network Usage: TODO");

		ImGui::End();
	}
}
