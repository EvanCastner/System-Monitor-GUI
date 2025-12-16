#include "dashboard.hpp"

#include "imgui.h"
#include "implot.h"

#include <vector>
#include <cmath>


namespace ui {
	void render_dashboard() {
		static float t = 0.0f;
		static std::vector<float> cpu_history;
		static const int MAX_SAMPLES = 200;

		// Generate dummy CPU usage
		float cpu_usage = 50.0f + 40.0f * std::sin(t);
		t = 0.05f;

		cpu_history.push_back(cpu_usage);
		if (cpu_history.size() > MAX_SAMPLES) {
			cpu_history.erase(cpu_history.begin());
		}

		ImGui::Begin("System Moniter Dashboard");

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
